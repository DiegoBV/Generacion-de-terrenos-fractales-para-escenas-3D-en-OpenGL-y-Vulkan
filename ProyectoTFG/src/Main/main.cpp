#include <iostream>
#include <vector>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"
#include "Window.h"

Camera camera;
glm::dvec2 mCoord;

//calcula el desplazamiento del raton al moverlo y manda a la camara moverse en funcion de el
//callback llamado cada vez que movemos el raton. Solo manda mover a la camara cuando estamos clicando, 
//momento en el cual se actualiza mCoord (se llama a mouse) y se produce un desplazamiento
void motion(GLFWwindow* window, double xpos, double ypos) {
	glm::dvec2 mOffset = mCoord; // var. global (mCoord vale las coordenadas del raton)
	mCoord = glm::dvec2(xpos, Window::GetSingleton()->getWindowHeight() - ypos);//actualizamos mCoord invirtiendo el eje y
	mOffset = (mCoord - mOffset) * 0.05; // sensitivity. mOffset vale la diferencia (desplazamiento -> nuevas coord del raton - coord anteriores)
	camera.handleOrientation(mOffset.x, mOffset.y);//mandamos que se mueva con el desplazamiento
}

//actualiza las coordenadas del raton invirtiendo el eje y; callback llamado cada vez que clicamos
void mouse(GLFWwindow* window, double xpos, double ypos) {
	mCoord = glm::dvec2(xpos, Window::GetSingleton()->getWindowHeight() - ypos);
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double deltaTime = TimeManager::GetSingleton()->getDeltaTime();

	switch (key) {
	case 'W'://si pulsamos "w" acercamos la camara
		camera.handleMovement(Camera_Movement::FORWARD, deltaTime);
		break;
	case 'S'://"s" la alejamos
		camera.handleMovement(Camera_Movement::BACKWARD, deltaTime);
		break;
	case 'D'://derecha
		camera.handleMovement(Camera_Movement::RIGHT, deltaTime);
		break;
	case 'A'://izquierda
		camera.handleMovement(Camera_Movement::LEFT, deltaTime);
		break;
	default:
		break;
	}//switch
}

int main()
{
	TimeManager* timeManager = TimeManager::GetSingleton();
	ApplicationManager* appManager = ApplicationManager::GetSingleton();
	Window* window = Window::GetSingleton();
	std::vector<Manager*> managers;
	managers.push_back(timeManager);
	managers.push_back(appManager);

	window->init();
	for (Manager* manager : managers) manager->init();

	std::list<RenderShader*> renderShaders;
	std::list<ComputeShader*> computeShaders;

	RenderShader renderShader = RenderShader();
	renderShader.init("vertex.c", "fragment.c");
	renderShader.use();
	appManager->addRenderShader(&renderShader);
	renderShaders.push_back(&renderShader);

	ComputeShader computeShader = ComputeShader();
	computeShader.init("compute.c");
	computeShader.use();
	appManager->addComputeShader(&computeShader);
	computeShaders.push_back(&computeShader);

	appManager->GetSingleton()->setUpGraphicsPipeline();

	// Callback registration
	window->setKeyCallback(key);
	window->setCursorCallback(motion);

	UniformBufferObject ubo;
	ubo.resolution = { window->getWindowWidth(), window->getWindowHeight() };
	ubo.worldUp = camera.getWorldUp();

	StorageBufferObject ssbo;
	ssbo.position = { 10.0f, 0.0f, 0.0f };
	computeShader.setSSBO(ssbo);

	// render loop
	// -----------
	while (!window->shouldClose())
	{
		ubo.cameraEye = camera.getEye();
		ubo.cameraFront = camera.getFront();
		ubo.viewMat = transpose(camera.getViewMatrix());
		ubo.time = timeManager->getTimeSinceBeginning();

		// update
		for (ComputeShader* shader : computeShaders) {
			//shader->setSSBO(ssbo);
			shader->use();
			std::cout << shader->getSSBO().position.x << std::endl;
		}

		// render
		for (RenderShader* shader : renderShaders) {
			shader->setUBO(ubo);
			shader->use();
		}

		window->update();
		for (Manager* manager : managers) manager->update();
	}

	for (RenderShader* shader : renderShaders) {
		shader->release();
	}
	for (ComputeShader* shader : computeShaders) {
		shader->release();
	}

	appManager->waitUntilFinishEverything();
	appManager->ShutDownSingleton();
	timeManager->ShutDownSingleton();
	window->ShutDownSingleton();

	system("pause");
	return 0;
}