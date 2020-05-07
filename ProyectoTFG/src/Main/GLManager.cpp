#if defined(GL_DEBUG) || defined(GL_RELEASE)

#include "GLManager.h"
#include <glad/glad.h>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "GLComputeShader.h"
#include "GLRenderShader.h"

GLManager* GLManager::instance = nullptr;

GLManager* GLManager::GetSingleton()
{
	if (instance == nullptr) instance = new GLManager();
	return instance;
}

void GLManager::ShutDownSingleton()
{
	if (instance != nullptr) {
		instance->release();
		delete instance; instance = nullptr;
	}
}

void GLManager::init()
{
	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return;
	}
	// TODO: EN UN FUTURO ESTO COINCIDIRA CON EL NEAR PLANE!!!
	float vertices[] = {
		 1.0f,  1.0f, 0.0f,  // top right
		 1.0f, -1.0f, 0.0f,  // bottom right
		-1.0f, -1.0f, 0.0f,  // bottom left
		-1.0f,  1.0f, 0.0f   // top left 
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	glEnable(GL_DEPTH_TEST);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	model = GLModel(MODEL_PATH);
	modelShader = nullptr;
}

void GLManager::update()
{
	for (GLComputeShader* shader : computeShaders) {
		shader->use();
	}
}

void GLManager::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (GLRenderShader* shader : renderShaders) {
		shader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}
	if (modelShader != nullptr) {
		modelShader->use();
		model.Draw(modelShader);
	}
}

void GLManager::release()
{
}

void GLManager::waitUntilFinishEverything()
{
}

GLManager::GLManager()
{
}

GLManager::~GLManager()
{
}

#endif
