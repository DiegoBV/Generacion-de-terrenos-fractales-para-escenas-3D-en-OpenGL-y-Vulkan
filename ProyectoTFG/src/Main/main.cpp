#include <iostream>
#include <vector>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"
#include "Window.h"

Camera camera;
glm::dvec2 mCoord;
StorageBufferObject ssbo;

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

	int state = glfwGetKey(window, key);

	if (state == GLFW_PRESS) {
		switch (key) {
		case 'W'://si pulsamos "w" acercamos la camara
			ssbo.velocity = 1.0f;
			ssbo.direction.z = 1.0f;
			ssbo.direction.x = 0.0f;
			ssbo.direction.y = 0.0f;
			break;
		case 'S'://"s" la alejamos
			ssbo.velocity = 1.0f;
			ssbo.direction.z = -1.0f;
			ssbo.direction.x = 0.0f;
			ssbo.direction.y = 0.0f;
			break;
		case 'D'://derecha
			ssbo.velocity = 1.0f;
			ssbo.direction.x = 1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.y = 0.0f;
			break;
		case 'A'://izquierda
			ssbo.velocity = 1.0f;
			ssbo.direction.x = -1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.y = 0.0f;
			break;
		case 'Q':
			ssbo.velocity = 3.0f;
			ssbo.direction.y = 1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.x = 0.0f;
			break;
		case 'E':
			ssbo.velocity = 3.0f;
			ssbo.direction.y = -1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.x = 0.0f;
			break;
		case GLFW_KEY_UP:
			camera.handleMovement(Camera_Movement::FORWARD, deltaTime);
			break;
		case GLFW_KEY_DOWN:
			camera.handleMovement(Camera_Movement::BACKWARD, deltaTime);
			break;
		case GLFW_KEY_RIGHT:
			camera.handleMovement(Camera_Movement::RIGHT, deltaTime);
			break;
		case GLFW_KEY_LEFT:
			camera.handleMovement(Camera_Movement::LEFT, deltaTime);
			break;
		default:
			break;
		}//switch
	}
	else if (state == GLFW_RELEASE)
		ssbo.direction = { 0.0f,0.0f,0.0f };
}

int main()
{
	TimeManager* timeManager = TimeManager::GetSingleton();
	ApplicationManager* appManager = ApplicationManager::GetSingleton();
	Window* window = Window::GetSingleton();

	window->init();
	timeManager->init();
	appManager->init();

	std::list<RenderShader*> renderShaders;

	RenderShader renderShader = RenderShader();
	renderShader.init("vertex.c", "fragment.c");
	renderShader.use();
	appManager->addRenderShader(&renderShader);
	renderShaders.push_back(&renderShader);

	ComputeShader computeShader = ComputeShader();
	computeShader.init("compute.c");
	computeShader.use();
	appManager->addComputeShader(&computeShader);

	appManager->GetSingleton()->setUpGraphicsPipeline();

	// Callback registration
	window->setKeyCallback(key);
	window->setCursorCallback(motion);

	UniformBufferObject ubo;
	ubo.resolution = { window->getWindowWidth(), window->getWindowHeight() };
	ubo.worldUp = camera.getWorldUp();

	ssbo.velocity = 1.0f;
	ssbo.position = { 0.0f, 3.0f, 50.0f };
	ssbo.direction = { 0.0f, 0.0f, 0.0f };
	computeShader.setSSBO(ssbo);

	// render loop
	// -----------
	while (!window->shouldClose())
	{
		// input callbacks...
		window->update();
		timeManager->update();

		// update
		ssbo.deltaTime = timeManager->getDeltaTime();
		computeShader.setSSBO(ssbo);
		appManager->update();
		ssbo = computeShader.getSSBO();
		std::cout << "x: " << ssbo.debug.x << "y: " << ssbo.debug.y << "z: " << ssbo.debug.z << std::endl;

		ubo.cameraEye = camera.getEye();
		ubo.cameraFront = camera.getFront();
		ubo.viewMat = transpose(camera.getViewMatrix());
		ubo.time = timeManager->getTimeSinceBeginning();
		ubo.playerPos = ssbo.position;
		for (RenderShader* shader : renderShaders) {
			shader->setUBO(ubo);
		}

		appManager->render();
	}

	for (RenderShader* shader : renderShaders) {
		shader->release();
	}
	
	computeShader.release();

	appManager->waitUntilFinishEverything();
	appManager->ShutDownSingleton();
	timeManager->ShutDownSingleton();
	window->ShutDownSingleton();

	system("pause");
	return 0;
}