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

//gltf ������ ���������� �ε� �Ǿ����� Ȯ���մϴ�.
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

//gltf���Ͽ� ����� bytebuffer�� ���� �����ͷ� ��ȯ�Ͽ� ������ݴϴ�.
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

				std::cout << "componentType : " << accessor.componentType
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
						model.bufferViews[accessor.bufferView].byteLength+37)
					<< std::endl;
				print_buffer(
					model.buffers[model.bufferViews[accessor.bufferView].buffer].data,
					model.bufferViews[accessor.bufferView].byteOffset,
					model.bufferViews[accessor.bufferView].byteLength,
					accessor.componentType, attrib.first);

			}
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
