#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "json.hpp"
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

//원하는 vertex의 정보들을 입력합니다. out.gltf는 이 정보들이 저장됩니다.
void make_buffers(json &inputGltf)
{
	int vertexCount = 3;
	std::string pars = "[{\"op\": \"replace\", \"path\" : \"/count\", \"value\" : " + std::to_string(vertexCount)+ "}]";
	json j_patch = json::parse(pars);
	auto &acc0 = inputGltf["/accessors/0"_json_pointer];
	acc0 = acc0.patch(j_patch);
	auto &acc1 = inputGltf["/accessors/1"_json_pointer];
	acc1 = acc1.patch(j_patch);
	auto &acc2 = inputGltf["/accessors/2"_json_pointer];
	acc2 = acc2.patch(j_patch);
	auto &acc3 = inputGltf["/accessors/3"_json_pointer];
	acc3 = acc3.patch(j_patch);


	unsigned short idx[] = { 0,1,2 };
	float vtx[] = {
	   0.5f,  0.5f,  0.0f,          // 0th vertex position
	  -0.5f, -0.5f,  0.0f,          // 1st vertex position
	   0.5f, -0.5f,  0.0f,
	};

	float normal[] = {
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f,
		0.0f, 1.0f, -0.0f
	};
	float color[] = {
	0.0f, 1.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f, 1.0f
	};




	int length = sizeof(idx) + sizeof(vtx) + sizeof(color) + sizeof(normal);
	int a = 0;
	std::cout << "offset : " << a << " length : " << sizeof(idx) << std::endl;
	pars = "[{\"op\": \"replace\", \"path\" : \"/byteOffset\", \"value\" : " + std::to_string(a) +
		"},{\"op\": \"replace\", \"path\" : \"/byteLength\", \"value\" : " + std::to_string(sizeof(idx)) +
		"}]";
	j_patch = json::parse(pars);
	auto &bufview0 = inputGltf["/bufferViews/0"_json_pointer];
	bufview0 = bufview0.patch(j_patch);

	a = sizeof(idx);
	std::cout << "offset : " << a << " length : " << sizeof(vtx) << std::endl;
	pars = "[{\"op\": \"replace\", \"path\" : \"/byteOffset\", \"value\" : " + std::to_string(a) +
		"},{\"op\": \"replace\", \"path\" : \"/byteLength\", \"value\" : " + std::to_string(sizeof(vtx)) +
		"}]";
	j_patch = json::parse(pars);
	auto &bufview1 = inputGltf["/bufferViews/1"_json_pointer];
	bufview1 = bufview1.patch(j_patch);

	a += sizeof(vtx);
	std::cout << "offset : " << a << " length : " << sizeof(normal) << std::endl;
	pars = "[{\"op\": \"replace\", \"path\" : \"/byteOffset\", \"value\" : " + std::to_string(a) +
		"},{\"op\": \"replace\", \"path\" : \"/byteLength\", \"value\" : " + std::to_string(sizeof(normal)) +
		"}]";
	j_patch = json::parse(pars);
	auto &bufview2 = inputGltf["/bufferViews/2"_json_pointer];
	bufview2 = bufview2.patch(j_patch);

	a += sizeof(normal);
	std::cout << "offset : " << a << " length : " << sizeof(color) << std::endl;
	pars = "[{\"op\": \"replace\", \"path\" : \"/byteOffset\", \"value\" : " + std::to_string(a) +
		"},{\"op\": \"replace\", \"path\" : \"/byteLength\", \"value\" : " + std::to_string(sizeof(color)) +
		"}]";
	j_patch = json::parse(pars);
	auto &bufview3 = inputGltf["/bufferViews/3"_json_pointer];
	bufview3 = bufview3.patch(j_patch);

	a += sizeof(color);

	std::vector<unsigned char> out(length);
	unsigned char* p_buffer1 = &out[0];
	for (unsigned short i = 0; i < sizeof(idx) / sizeof(unsigned short); i++) {
		*p_buffer1++ = (idx[i] & 0xFF);
		*p_buffer1++ = idx[i] >> 8;
	}
	for (unsigned short i = 0; i < sizeof(vtx) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = vtx[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];

	}
	for (unsigned short i = 0; i < sizeof(normal) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = normal[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];
	}
	for (unsigned short i = 0; i < sizeof(color) / sizeof(float); i++) {
		unsigned char buf2[sizeof(float)];
		float *p2 = (float *)&buf2[0];
		*p2 = color[i];
		*p_buffer1++ = buf2[0];
		*p_buffer1++ = buf2[1];
		*p_buffer1++ = buf2[2];
		*p_buffer1++ = buf2[3];
	}
	std::string newUri = tinygltf::base64_encode(&out[0], length);
	newUri = "data:application/octet-stream;base64," + newUri;
	pars = "[{\"op\": \"replace\", \"path\" : \"/byteLength\", \"value\" : " + std::to_string(length) +
		"},{\"op\": \"replace\", \"path\" : \"/uri\", \"value\" : \"" + newUri +
		"\"}]";
	j_patch = json::parse(pars);
	auto &buf = inputGltf["/buffers/0"_json_pointer];
	buf = buf.patch(j_patch);

	std::cout << newUri << '\n';
}



int main(int argc, char **argv) {
	tinygltf::Model model;

	std::ofstream outFile("out.gltf");

	std::string filename =
		"testTri.gltf";

	if (argc > 1) {
		filename = argv[1];
	}
	std::ifstream inputFile(filename);
	json inputGltf;
	json outGltf;
	inputFile >> inputGltf;

	make_buffers(inputGltf);

	loadModel(model, filename);

	outGltf = inputGltf;
	outFile << outGltf;
	return 0;
}