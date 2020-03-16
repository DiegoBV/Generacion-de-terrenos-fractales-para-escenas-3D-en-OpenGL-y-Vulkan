#pragma once
#include <list>

class GLRenderShader;
class GLComputeShader;

class GLManager
{
private:
	static GLManager* instance;
	std::list<GLRenderShader*> renderShaders;
	std::list<GLComputeShader*> computeShaders;

	unsigned int VBO, VAO, EBO;

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
};

