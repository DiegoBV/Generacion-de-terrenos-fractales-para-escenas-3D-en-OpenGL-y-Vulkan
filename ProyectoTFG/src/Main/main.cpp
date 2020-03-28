#include <iostream>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"
#include "Window.h"
#include "PlayableSphere.h"
#include <math.h>
#include <vector>

Camera camera;
glm::dvec2 mCoord;
float pivotOffset = 2.0f;
PlayableSphere player;

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
	int state = glfwGetKey(window, key);
	if (state == GLFW_PRESS) {
		player.handleMovement(key, camera.getFront());
	}
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
	ubo.playerColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

	player = PlayableSphere({ 0.0f, -0.8f, 0.0f }, 5.0f);

	// quitar esto de aqui y llevarlo a la clase playable object
	StorageBufferObject ssbo = player.getSSBO();
	ssbo.velocity = glm::vec3(0, 0, 0);
	ssbo.radius = 0.05f;
	ssbo.position = { 0.0f, 0.75f, 50.0f };
	std::vector<glm::vec3> dirs = player.getHitboxPoints();
	std::copy(dirs.begin(), dirs.end(), ssbo.collisionDirs);
	ssbo.mass = 1.5f;
	player.setSSBO(ssbo);

	computeShader.setSSBO(ssbo);
	// render loop
	// -----------
	while (!window->shouldClose())
	{
		// input callbacks...
		window->update();
		timeManager->update();
		player.update(timeManager->getDeltaTime());

		// update
		computeShader.setSSBO(player.getSSBO());
		appManager->update();
		player.setSSBO(computeShader.getSSBO());

		camera.pivotTarget(player.getSSBO().position, pivotOffset);

		ubo.cameraEye = camera.getEye();
		ubo.cameraFront = camera.getFront();
		ubo.viewMat = transpose(camera.getViewMatrix());
		ubo.time = timeManager->getTimeSinceBeginning();
		ubo.playerPos = player.getSSBO().position;
		ubo.playerRadius = player.getSSBO().radius;

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