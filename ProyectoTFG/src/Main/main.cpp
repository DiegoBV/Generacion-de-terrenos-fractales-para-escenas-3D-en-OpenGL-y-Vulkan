#include <iostream>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"
#include "Window.h"
#include "PlayableSphere.h"
#include <math.h>
#include <vector>

struct InitApplicationInfo {
	std::string vertexName;
	std::string fragmentName;
	std::string computeName;
	bool terrain;
	bool freeCamera;
	bool explorationMode;
};

Camera camera;
glm::dvec2 mCoord;
float pivotOffset = 2.0f;
bool debug = false;
PlayableSphere player;

InitApplicationInfo appInfo;

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
		if (key == 'F' && !appInfo.explorationMode) appInfo.freeCamera = !appInfo.freeCamera;
		//DEBUG
		else if (key == 'Z') pivotOffset -= 0.1f;
		else if (key == 'X') pivotOffset += 0.1f;
		else if (key == 'C') player.setRadius(player.getRadius() + 0.005f);
		else if (key == 'V') player.setRadius(player.getRadius() - 0.005f);
		else if (key == 'H') debug = !debug;
		//DEBUG

		else {
			if (appInfo.freeCamera)
				camera.handleMovement(key);
			else
				player.handleMovement(key, camera.getFront(), appInfo.terrain);
		}
	}
	else player.resetKeyDirection();
}

void init() {
	TimeManager* timeManager = TimeManager::GetSingleton();
	ApplicationManager* appManager = ApplicationManager::GetSingleton();
	Window* window = Window::GetSingleton();

	window->init();
	timeManager->init();
	appManager->init();
}

void release() {
	ApplicationManager::GetSingleton()->waitUntilFinishEverything();
	ApplicationManager::GetSingleton()->ShutDownSingleton();
	TimeManager::GetSingleton()->ShutDownSingleton();
	Window::GetSingleton()->ShutDownSingleton();
}

void runApplication(const std::string& vertex, const std::string& fragment, const std::string& compute, bool terrain) {

	TimeManager* timeManager = TimeManager::GetSingleton();
	ApplicationManager* appManager = ApplicationManager::GetSingleton();
	Window* window = Window::GetSingleton();

	std::list<RenderShader*> renderShaders;

	ComputeShader computeShader = ComputeShader();
	computeShader.init(compute);
	computeShader.use();
	appManager->addComputeShader(&computeShader);

	RenderShader renderShader = RenderShader();
	renderShader.init(vertex, fragment);
	renderShader.use();
	appManager->addRenderShader(&renderShader);
	renderShaders.push_back(&renderShader);

	RenderShader modelShader = RenderShader();
	modelShader.init("vmodel.c", "fmodel.c");
	modelShader.use();
	appManager->setModelShader(&modelShader);
	renderShaders.push_back(&modelShader);

	appManager->GetSingleton()->setUpGraphicsPipeline();

	// Callback registration
	window->setKeyCallback(key);
	window->setCursorCallback(motion);

	UniformBufferObject ubo;
	ubo.resolution = { window->getWindowWidth(), window->getWindowHeight() };
	ubo.worldUp = camera.getWorldUp();
	ubo.playerColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

	glm::vec3 gravityDirection = { 0.0f, -1.0f, 0.0f };
	player = PlayableSphere(gravityDirection, { 0.0f, 0.0f, 0.0f }, { 0.0f, 3.0f, 0.0f }, 1.5f, 1.5f, 5.0f, 0.1f, 0.01f);

	computeShader.setSSBO(player.getSSBO());

	glm::mat4 unityMatrix = glm::mat4(1.0f);
	glm::mat4 model = unityMatrix;
	ubo.projection = glm::perspective(glm::radians(camera.getZoom()), (float)window->getWindowWidth() / (float)window->getWindowHeight(), 0.1f, 100.0f);

	float lastCameraYaw = 0.0f;
	float lastDepthAdvance = 0.0f;
	float lastLateralAdvance = 0.0f;
	float rotationAngle = 300.0f;
	glm::vec3 lastPlayerPosition;

	// render loop
	// -----------
	while (!window->shouldClose())
	{
		// input callbacks...
		window->update();
		timeManager->update();
		if (!appInfo.freeCamera) player.update(timeManager->getDeltaTime());

		// update
		if (!terrain) {
			gravityDirection = glm::vec3(0, 0, 0) - player.getPosition();
			player.setGravityDirection(gravityDirection);
		}
		computeShader.setSSBO(player.getSSBO());
		appManager->update();
		player.setSSBO(computeShader.getSSBO());

		if(!appInfo.freeCamera) camera.pivotTarget(player.getSSBO().position, pivotOffset);

		if (!appInfo.freeCamera) {
			lastPlayerPosition = player.getSSBO().position;
			lastDepthAdvance += player.getDirection().z * rotationAngle * TimeManager::GetSingleton()->getDeltaTime();
			lastLateralAdvance += (-player.getDirection().x) * rotationAngle * TimeManager::GetSingleton()->getDeltaTime();
			lastCameraYaw = camera.getYaw();
		}

		model = glm::translate(unityMatrix, lastPlayerPosition); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-lastCameraYaw - 180.0f), { 0, 1, 0 });
		model = glm::rotate(model, glm::radians(lastDepthAdvance), { 0, 0, 1 });
		model = glm::rotate(model, glm::radians(lastLateralAdvance), { 1, 0, 0 });
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down

		ubo.cameraEye = camera.getEye();
		ubo.cameraFront = camera.getFront();
		ubo.viewMat = transpose(camera.getViewMatrix());
		ubo.time = timeManager->getTimeSinceBeginning();
		ubo.playerPos = lastPlayerPosition;
		ubo.playerRadius = player.getSSBO().radius;
		ubo.fractalRotation = player.getSSBO().fractalRotation;
		ubo.debug = debug;
		ubo.model = model;

		for (RenderShader* shader : renderShaders) {
			shader->setUBO(ubo);
		}

		appManager->render();
	}

	for (RenderShader* shader : renderShaders) {
		shader->release();
	}

	computeShader.release();
}

char menu() {
	char option = ' ';
	do {
		system("cls");
		std::cout << "Write the desired map" << std::endl;
		std::cout << "1: Snow terrain" << std::endl;
		std::cout << "2: Mandelbulb" << std::endl;
		std::cout << "3: Mandelbox" << std::endl;
		std::cout << "4: Debug scene" << std::endl;
		std::cout << "Q: Exit application" << std::endl;

		std::cout << "Enter your selection: ";
		std::cin >> option;

		option = std::toupper(option);
		if (option == 'Q') {
			break;
		}

	} while(option <= '0' || option > '4');

	return option;
}

InitApplicationInfo setAppInfo(const char& option) {

	switch (option)
	{
	case '1':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "snowTerrainFragment.c";
		appInfo.computeName = "snowTerrainCompute.c";
		appInfo.terrain = true;
		appInfo.freeCamera = false;
		appInfo.explorationMode = false;
		break;
	case '2':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "mandelbulbFragment.c";
		appInfo.computeName = "mandelbulbCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = false;
		appInfo.explorationMode = false;
		break;
	case '3':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "mandelboxFragment.c";
		appInfo.computeName = "mandelboxCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = true;
		appInfo.explorationMode = true;
		break;
	case '4':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "scene0fragment.c";
		appInfo.computeName = "scene0Compute.c";
		appInfo.terrain = true;
		appInfo.freeCamera = false;
		appInfo.explorationMode = false;
		break;
	default:
		break;
	}

	return appInfo;
}

int main()
{
	char selectedOption = menu();
	while (selectedOption != 'Q')
	{
		InitApplicationInfo appInfo = setAppInfo(selectedOption);
		init();
		runApplication(appInfo.vertexName, appInfo.fragmentName, appInfo.computeName, appInfo.terrain);
		release();

		selectedOption = menu();
	}

	return 0;
}