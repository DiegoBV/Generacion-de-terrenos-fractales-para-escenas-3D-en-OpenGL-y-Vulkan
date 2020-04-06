#pragma once

#if defined(GL_DEBUG) || defined(GL_RELEASE)
#include "GLModel.h"
using Model = GLModel;
#elif defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanModel.h"
#include "VulkanModel.h"
using Model = VulkanModel;
#endif