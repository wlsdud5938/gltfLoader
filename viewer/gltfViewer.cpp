///// main.cpp
///// OpenGL 3+, GLSL 1.20, GLEW, GLFW3

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include "../common/vec.hpp"
#include "../common/transform.hpp" 
#include "window.h"
#include "Camera.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"
#define BUFFER_OFFSET(i) ((char *)NULL + (i))
////////////////////////////////////////////////////////////////////////////////
/// 쉐이더 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
GLuint  program;          // 쉐이더 프로그램 객체의 레퍼런스 값
GLint   loc_a_position;   // attribute 변수 a_position 위치
GLint   loc_a_color;      // attribute 변수 a_color 위치
GLint   loc_u_PVM;        // uniform 변수 u_PVM 위치


GLuint create_shader_from_file(const std::string& filename, GLuint shader_type);
void init_shader_program();
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
/// 변환 관련 변수 및 함수
////////////////////////////////////////////////////////////////////////////////
kmuvcl::math::mat4x4f     mat_model, mat_view, mat_proj;
kmuvcl::math::mat4x4f     mat_PVM;

float   g_translate_x = 0.0, g_translate_y = 0.0, g_translate_z = 0.0;
bool    g_is_animation = false;

void set_transform();
////////////////////////////////////////////////////////////////////////////////

void render_object();           // rendering 함수: 물체(gltf모델)를 렌더링하는 함수.
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
/// 카메라 및 뷰포트 관련 변수
////////////////////////////////////////////////////////////////////////////////
Camera  camera;
float   g_aspect = 1.0f;
////////////////////////////////////////////////////////////////////////////////


// GLSL 파일을 읽어서 컴파일한 후 쉐이더 객체를 생성하는 함수
GLuint create_shader_from_file(const std::string& filename, GLuint shader_type)
{
  GLuint shader = 0;

  shader = glCreateShader(shader_type);

  std::ifstream shader_file(filename.c_str());
  std::string shader_string;

  shader_string.assign(
    (std::istreambuf_iterator<char>(shader_file)),
    std::istreambuf_iterator<char>());

  const GLchar * shader_src = shader_string.c_str();
  glShaderSource(shader, 1, (const GLchar **)&shader_src, NULL);
  glCompileShader(shader);

  return shader;
}

// vertex shader와 fragment shader를 링크시켜 program을 생성하는 함수
void init_shader_program()
{
  GLuint vertex_shader
    = create_shader_from_file("./shader/vertex.glsl", GL_VERTEX_SHADER);

  std::cout << "vertex_shader id: " << vertex_shader << std::endl;
  assert(vertex_shader != 0);

  GLuint fragment_shader
    = create_shader_from_file("./shader/fragment.glsl", GL_FRAGMENT_SHADER);

  std::cout << "fragment_shader id: " << fragment_shader << std::endl;
  assert(fragment_shader != 0);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  std::cout << "program id: " << program << std::endl;
  assert(program != 0);

  loc_u_PVM = glGetUniformLocation(program, "u_PVM");

  loc_a_position = glGetAttribLocation(program, "a_position");
  loc_a_color = glGetAttribLocation(program, "a_color");
}


void set_transform()
{
  // set camera transformation
  kmuvcl::math::vec3f eye     = camera.position();
  kmuvcl::math::vec3f up      = camera.up_direction();
  kmuvcl::math::vec3f center  = eye + camera.front_direction();

  std::cout << "eye:    " << eye << std::endl;
  std::cout << "up:     " << up << std::endl;
  std::cout << "center: " << center << std::endl;

  mat_view = kmuvcl::math::lookAt(eye[0], eye[1], eye[2],
                                  center[0], center[1], center[2],
                                  up[0], up[1], up[2]);

  float n = camera.near();
  float f = camera.far();

  if (camera.mode() == Camera::kOrtho)
  {
    float l = camera.left();
    float r = camera.right();
    float b = camera.bottom();
    float t = camera.top();

    std::cout << "(camera.mode() == Camera::kOrtho)" << std::endl;
    std::cout << "(l, r, b, t, n, f): " << l << ", " << r << ", " << b << ", " << t << ", " << n << ", " << f << std::endl;

    mat_proj = kmuvcl::math::ortho(l, r, b, t, n, f);
  }
  else if (camera.mode() == Camera::kPerspective)
  {
    std::cout << "(camera.mode() == Camera::kPerspective)" << std::endl;
    std::cout << "(fov, aspect, n, f): " << camera.fovy() << ", " << g_aspect << ", " << n << ", " << f << std::endl;

    mat_proj = kmuvcl::math::perspective(camera.fovy(), g_aspect, n, f);
  }

  // set object transformation
  mat_model = kmuvcl::math::translate(g_translate_x, g_translate_y, g_translate_z);
}

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);

  g_aspect = (float)width / (float)height;
}

//TinyGLTF를 이용하여 gltf모델을 로딩하는 함수
bool loadModel(tinygltf::Model &model, const char *filename) {
  tinygltf::TinyGLTF loader;
  std::string err;
  std::string warn;

  bool res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
  if (!warn.empty()) {
    std::cout << "WARN: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "ERR: " << err << std::endl;
  }

  if (!res)
    std::cout << "Failed to load glTF: " << filename << std::endl;
  else
    std::cout << "Loaded glTF: " << filename << std::endl;

  return res;
}

//TinyGLTF를 이용하여 로드한 모델을 파싱하고 버텍스 쉐이더에 연결하는 함수
std::map<int, GLuint> bindMesh(std::map<int, GLuint> vbos,
                               tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < model.bufferViews.size(); ++i) {
    const tinygltf::BufferView &bufferView = model.bufferViews[i];
    if (bufferView.target == 0) {
      std::cout << "WARN: bufferView.target is zero" << std::endl;
      continue; 
    }

    tinygltf::Buffer buffer = model.buffers[bufferView.buffer];
    std::cout << "bufferview.target " << bufferView.target << std::endl;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    vbos[i] = vbo;
    glBindBuffer(bufferView.target, vbo);

    std::cout << "buffer.data.size = " << buffer.data.size()
              << ", bufferview.byteOffset = " << bufferView.byteOffset
              << std::endl;

    glBufferData(bufferView.target, bufferView.byteLength,
                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
  }

  for (size_t i = 0; i < mesh.primitives.size(); ++i) 
  {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    for (auto &attrib : primitive.attributes) 
    {
      tinygltf::Accessor accessor = model.accessors[attrib.second];
      int byteStride =
          accessor.ByteStride(model.bufferViews[accessor.bufferView]);
      glBindBuffer(GL_ARRAY_BUFFER, vbos[accessor.bufferView]);

      int size = 1;
      if (accessor.type != TINYGLTF_TYPE_SCALAR) 
      {
        size = accessor.type;
      }
      
      if (attrib.first.compare("POSITION") == 0)
      {
        // 버텍스 쉐이더의 attribute 중 a_position 부분 활성화
        glEnableVertexAttribArray(loc_a_position);
        // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_position에 해당하는 attribute와 연결
        glVertexAttribPointer(loc_a_position, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
      } 

      if (attrib.first.compare("COLOR_0") == 0)
      { 
        // 버텍스 쉐이더의 attribute 중 a_color 부분 활성화
        glEnableVertexAttribArray(loc_a_color);
        // 현재 배열 버퍼에 있는 데이터를 버텍스 쉐이더 a_color 해당하는 attribute와 연결
        glVertexAttribPointer(loc_a_color, size, accessor.componentType,
                              accessor.normalized ? GL_TRUE : GL_FALSE,
                              byteStride, BUFFER_OFFSET(accessor.byteOffset));
      }
    }
  }
  return vbos;
}
// bind models
GLuint bindModel(tinygltf::Model &model) {
  
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);
  std::map<int, GLuint> vbos;
  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  bindMesh(vbos, model, model.meshes[model.nodes[scene.nodes[0]].mesh]);
  glBindVertexArray(0);
  // cleanup vbos
  for (size_t i = 0; i < vbos.size(); ++i) {
    glDeleteBuffers(1, &vbos[i]);
  }

  return vao;
}

void drawMesh(tinygltf::Model &model, tinygltf::Mesh &mesh) {
  for (size_t i = 0; i < mesh.primitives.size(); ++i) {
    tinygltf::Primitive primitive = mesh.primitives[i];
    tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

    glDrawElements(primitive.mode, indexAccessor.count,
                   indexAccessor.componentType,
                   BUFFER_OFFSET(indexAccessor.byteOffset));
  }
}

// recursively draw node and children nodes of model
void drawModel(GLuint vao, tinygltf::Model &model) {
  glBindVertexArray(vao);

  const tinygltf::Scene &scene = model.scenes[model.defaultScene];
  drawMesh(model, model.meshes[model.nodes[scene.nodes[0]].mesh]);
  glBindVertexArray(0);
}

// object rendering: 현재 scene은 삼각형 하나로 구성되어 있음.
void render_object(tinygltf::Model model,GLuint vao)
{
  glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // 특정 쉐이더 프로그램 사용
  glUseProgram(program);

  // Setting Proj * View * Model
  mat_PVM = mat_proj * mat_view * mat_model;
  glUniformMatrix4fv(loc_u_PVM, 1, GL_FALSE, mat_PVM);

  drawModel(vao, model);

  // 정점 attribute 배열 비활성화
  glDisableVertexAttribArray(loc_a_position);
  glDisableVertexAttribArray(loc_a_color);
  // 쉐이더 프로그램 사용해제
  glUseProgram(0);
}


int main(int argc, char **argv)
{
  GLFWwindow* window;

  // Initialize GLFW library
  if (!glfwInit()){
    std::cout << "Failed glfwInit!" << std::endl;
    return -1;
  }
  
  // Create a GLFW window containing a OpenGL context
  window = glfwCreateWindow(500, 500, "Hello Gltf", NULL, NULL);
  if (!window)
  {
    std::cout << "No window!" << std::endl;
    glfwTerminate();
    return -1;
  }

  // Make the current OpenGL context as one in the window
  glfwMakeContextCurrent(window);

  // Initialize GLEW library
  if (glewInit() != GLEW_OK)
    std::cout << "GLEW Init Error!" << std::endl;

  // Print out the OpenGL version supported by the graphics card in my PC
  std::cout << glGetString(GL_VERSION) << std::endl;

  init_shader_program();

  /// TODO: Set a callback for resizing the framebuffer 
  glfwSetFramebufferSizeCallback(window, frambuffer_size_callback);

  std::string filename = "out.gltf";
  if (argc > 1) {
		filename = argv[1];
	}
  tinygltf::Model model;
  if (!loadModel(model, filename.c_str())) return 0;

  GLuint vao = bindModel(model);
  
  std::cout << "start loop" << std::endl; 
  // Loop until the user closes the window
  while (!glfwWindowShouldClose(window))
  {
    // Poll for and process events
    glfwPollEvents();

    set_transform();
    render_object(model, vao);

    if (g_is_animation)
    {
      g_translate_x += 0.1f;
      if (g_translate_x > 1.0f)
        g_translate_x = -1.0f;
    }

    // Swap front and back buffers
    glfwSwapBuffers(window);
  }

  glfwTerminate();

  return 0;
}