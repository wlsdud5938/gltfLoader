//#include <GL/glew.h>

//#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
//#include <glm/gtc/matrix_transform.hpp>

//#include "shaders.h"
//#include "window.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "tiny_gltf.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool loadModel(tinygltf::Model &model, const std::string filename) {
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


void print_buffer(const std::vector<unsigned char>& buffer,
	int offset, int length,
	int componentType, const std::string attribName) {
	int next = 0;
	if (!attribName.compare("COLOR_0"))
		next = 4;
	else if (!attribName.compare("indices"))
		next = 0;
	else
		next = 3;
	if (componentType == TINYGLTF_COMPONENT_TYPE_BYTE) {
		//std::cout << buffer << std::endl; 
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE) {
		//std::cout << buffer << std::endl; 
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_SHORT) {
		const unsigned char* p_bytes = &(buffer[offset]);
		const short* p_short = reinterpret_cast<const short*>(p_bytes);
		int n = 0;
		for (size_t i = 0; i < length / sizeof(short); ++i) {
			std::cout << std::fixed;
			std::cout.precision(6);
			std::cout.setf(std::ios::showpoint);
			std::cout << p_short[i] << " ";
			n++;
			if (n == next)
			{
				n = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT) {
		const unsigned char* p_bytes = &(buffer[offset]);
		const unsigned short* p_short = reinterpret_cast<const unsigned short*>(p_bytes);
		int n = 0;
		for (size_t i = 0; i < length / sizeof(unsigned short); ++i) {
			std::cout << p_short[i] << " ";
			n++;
			if (n == next)
			{
				n = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_INT) {
		//std::cout << buffer << std::endl; 
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT) {
		//std::cout << buffer << std::endl; 
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {
		const unsigned char* p_bytes = &(buffer[offset]);
		const float* p_floats = reinterpret_cast<const float*>(p_bytes);
		int n = 0;
		for (size_t i = 0; i < length / sizeof(float); ++i) {
			std::cout << std::fixed;
			std::cout.precision(6);
			std::cout.setf(std::ios::showpoint);
			std::cout << p_floats[i] << " ";
			n++;
			if (n == next)
			{
				n = 0;
				std::cout << std::endl;
			}
		}
		std::cout << std::endl;
	}
	else if (componentType == TINYGLTF_COMPONENT_TYPE_DOUBLE) {
		//std::cout << buffer << std::endl; 
	}
}

std::vector<unsigned char> make_buffers(int* length)
{

	unsigned short idx[] = { 0,1,2 };
	float vtx[] = {
		-1.0f, 0.0f,  1.0f,
		 1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, -1.0f
	};
	float color[] = {
		0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f
	};
	float normal[] = {
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f
	};
	unsigned char buf2[sizeof(float)];

	float *p2 = (float *)&buf2[0];
	*p2 = -1.0f;

	*length = sizeof(idx) + sizeof(vtx) + sizeof(color) + sizeof(normal);
	int a = 0;
	std::cout << "offset : " << a << " length : " << sizeof(idx) << std::endl;
	a = sizeof(idx);
	std::cout << "offset : " << a << " length : " << sizeof(vtx) << std::endl;
	a += sizeof(vtx);
	std::cout << "offset : " << a << " length : " << sizeof(normal) << std::endl;
	a += sizeof(normal);
	std::cout << "offset : " << a << " length : " << sizeof(color) << std::endl;
	a += sizeof(color);

	std::vector<unsigned char> out(*length);
	unsigned char* p_buffer1 = &out[0];
	for (unsigned int i = 0; i < sizeof(idx) / sizeof(unsigned short); i++) {
		*p_buffer1++ = (idx[i] & 0xFF);
		*p_buffer1++ = idx[i] >> 8;
	}
	std::string newUri = tinygltf::base64_encode(&out[0], 12);
	for (unsigned int i = 0; i < sizeof(vtx) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = vtx[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];

	}
	for (unsigned int i = 0; i < sizeof(normal) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = normal[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];
	}
	for (unsigned int i = 0; i < sizeof(color) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = color[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];
	}
	return out;
}

void dbgModel(tinygltf::Model &model) {
	for (auto &node : model.nodes) {
		std::cout << "node name : " << node.name << std::endl;
		std::cout << "rotation : " << std::endl;
		for (auto rotation : node.rotation)
			std::cout << rotation << std::endl;
		std::cout << "translation : " << std::endl;
		for (auto translation : node.translation)
			std::cout << translation << std::endl;
		std::cout << "scale : " << std::endl;
		for (auto scale : node.scale)
			std::cout << scale << std::endl;
	}
	for (auto &mesh : model.meshes) {
		std::cout << "mesh : " << mesh.name << std::endl;
		for (auto &primitive : mesh.primitives) {
			const tinygltf::Accessor &indexAccessor =
				model.accessors[primitive.indices];

			std::cout << "indexaccessor: count " << indexAccessor.count << ", type "
				<< indexAccessor.componentType << std::endl;

			//tinygltf::Material &mat = model.materials[primitive.material];
			/*for (auto &mats : model.materials[primitive.material].values) {
			  std::cout << "mat : " << mats.first.c_str() << std::endl;
			}*/

			for (auto &image : model.images) {
				std::cout << "image name : " << image.uri << std::endl;
				std::cout << "  size : " << image.image.size() << std::endl;
				std::cout << "  w/h : " << image.width << "/" << image.height
					<< std::endl;
			}

			std::cout << "indices : " << primitive.indices << std::endl;
			std::cout << "mode     : "
				<< "(" << primitive.mode << ")" << std::endl;

			for (auto &attrib : primitive.attributes) {
				std::cout << "attribute : " << attrib.first.c_str() << std::endl;
				std::cout << "attributeValue : " << attrib.second << std::endl;
				tinygltf::Accessor accessor = model.accessors[attrib.second];

				std::cout << "color componentType : " << accessor.componentType
					<< std::endl;
				std::cout << "bufferoffset : " << accessor.byteOffset << std::endl;
				std::cout << "buffer : "
					<< model.bufferViews[accessor.bufferView].buffer
					<< std::endl;
				std::cout << "byteLength : "
					<< model.bufferViews[accessor.bufferView].byteLength
					<< std::endl;
				std::cout << "byteOffset : "
					<< model.bufferViews[accessor.bufferView].byteOffset
					<< std::endl;
				std::cout << "target : "
					<< model.bufferViews[accessor.bufferView].target
					<< std::endl;
				std::cout
					<< "buffers : "
					<< model.buffers[model.bufferViews[accessor.bufferView].buffer]
					.uri.substr(
						model.bufferViews[accessor.bufferView].byteOffset+37,
						model.bufferViews[accessor.bufferView].byteLength)
					<< std::endl;
				print_buffer(
					model.buffers[model.bufferViews[accessor.bufferView].buffer].data,
					model.bufferViews[accessor.bufferView].byteOffset,
					model.bufferViews[accessor.bufferView].byteLength,
					accessor.componentType, attrib.first);
			}
			int l = 0;

			std::vector<unsigned char> data = make_buffers(&l);
			std::string newUri = tinygltf::base64_encode(&data[0], l);
			std::cout << "make Uri : data:application/octet-stream;base64," << newUri << std::endl;
		}
	}
}



int main(int argc, char **argv) {
	tinygltf::Model model;

	std::string filename =
		"testTri.gltf";

	if (argc > 1) {
		filename = argv[1];
	}

	loadModel(model, filename);
	dbgModel(model);
	return 0;
}
