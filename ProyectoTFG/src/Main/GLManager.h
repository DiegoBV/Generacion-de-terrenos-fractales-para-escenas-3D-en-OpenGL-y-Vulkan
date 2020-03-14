#pragma once
#include "Manager.h"

class GLRenderShader;
class GLComputeShader;

class GLManager: public Manager 
{
private:
	static GLManager* instance;

	unsigned int VBO, VAO, EBO;

	GLManager();
	~GLManager();

public:
	static GLManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	virtual void update();
	virtual void release();
	virtual void waitUntilFinishEverything();
	void setUpGraphicsPipeline() {};
	void addRenderShader(GLRenderShader* shader) {};
	void addComputeShader(GLComputeShader* shader) {};
};

