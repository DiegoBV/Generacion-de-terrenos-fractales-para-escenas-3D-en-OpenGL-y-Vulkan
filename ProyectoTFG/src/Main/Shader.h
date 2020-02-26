#pragma once

#if defined(GL_DEBUG) || defined(GL_RELEASE)
#include "GLShader.h"
using Shader = GLShader;
#elif defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanShader.h"
using Shader = VulkanShader;
#endif