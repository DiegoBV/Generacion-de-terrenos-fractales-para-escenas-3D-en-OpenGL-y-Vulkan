#pragma once
#include <vulkan.hpp>
#include <string>
#include "Shader.h"

using namespace std;

class VulkanModel
{
private:
	void loadModel(string const& path);

public:
	VulkanModel(string const& path, bool gamma = false);
	~VulkanModel() {}

	void Draw(RenderShader* shader);
};