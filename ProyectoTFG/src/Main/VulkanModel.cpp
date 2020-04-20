#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanModel.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#include <stdexcept>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>
#include "VulkanManager.h"

VulkanModel::VulkanModel(string const& path, bool gamma)
{
	loadModel(path);
}

void VulkanModel::Draw(RenderShader* shader)
{
}

void VulkanModel::loadModel(string const& path)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
		throw std::runtime_error(warn + err);
	}

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};

			modelVertices.push_back(vertex);
			modelIndices.push_back(modelIndices.size());
		}
	}
}

#endif
