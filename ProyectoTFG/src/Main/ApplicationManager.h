#pragma once

#if defined(GL_DEBUG) || defined(GL_RELEASE)
#include "GLManager.h"
using ApplicationManager = GLManager;
#elif defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanManager.h"
using ApplicationManager = VulkanManager;
#endif