#pragma once
#include <list>
#include "GLModel.h"

class GLRenderShader;
class GLComputeShader;

class GLManager
{
private:
	static GLManager* instance;
	std::list<GLRenderShader*> renderShaders;
	std::list<GLComputeShader*> computeShaders;
	GLRenderShader* modelShader;

	unsigned int VBO, VAO, EBO;

	GLModel model;

	const std::string MODEL_PATH = "..\\Assets\\Models\\pacman\\pacman.obj";

	GLManager();
	~GLManager();

public:
	static GLManager* GetSingleton();
	static void ShutDownSingleton();

	void init();
	void update();
	void render();
	void release();
	void waitUntilFinishEverything();
	void setUpGraphicsPipeline() {};
	inline void addRenderShader(GLRenderShader* shader) { renderShaders.push_back(shader); };
	inline void addComputeShader(GLComputeShader* shader) { computeShaders.push_back(shader); };
	inline void setModelShader(GLRenderShader* shader) { modelShader = shader; };
};

