#pragma once

#if defined(GL_DEBUG) || defined(GL_RELEASE)
#include "GLRenderShader.h"
#include "GLComputeShader.h"
using RenderShader = GLRenderShader;
using ComputeShader = GLComputeShader;
#elif defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanRenderShader.h"
#include "VulkanComputeShader.h"
using RenderShader = VulkanRenderShader;
using ComputeShader = VulkanComputeShader;
#endif