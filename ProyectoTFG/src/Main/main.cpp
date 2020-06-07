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
	bool terrain;
	bool freeCamera;
	bool explorationMode;

	float cameraVelocity;
	float playerAcceleration;
	float collisionRadius;
	float pivotOffset;
	float gravityForce;

	glm::vec3 initialPosition;
	glm::vec3 modelScale;
	glm::vec3 cameraDirection;

	std::string vertexName;
	std::string fragmentName;
	std::string computeName;
};

Camera camera;
glm::dvec2 mCoord;
float pivotOffset = 2.0f;
float scaleIncrease = 0.005f;
bool debug = false;
PlayableSphere player;

InitApplicationInfo appInfo;

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

		appInfo.initialPosition = {0.0f, 1.2f, 0.0f};
		appInfo.playerAcceleration = 5.0f;
		appInfo.collisionRadius = 0.065f;
		appInfo.modelScale = {0.1f, 0.1f, 0.1f};
		appInfo.cameraDirection = {0.0f, 0.0f, -1.0f};
		appInfo.cameraVelocity = 1.0f;
		appInfo.pivotOffset = 1.7f;
		appInfo.gravityForce = 1.5f;
		break;
	case '2':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "oceanTerrainFragment.c";
		appInfo.computeName = "oceanTerrainCompute.c";
		appInfo.terrain = true;
		appInfo.freeCamera = true;
		appInfo.explorationMode = true;

		appInfo.initialPosition = { 0.0f, 100.0f, 0.0f };
		appInfo.playerAcceleration = 10.0f;
		appInfo.collisionRadius = 0.065f;
		appInfo.modelScale = { 0.1f, 0.1f, 0.1f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 500.0f;
		appInfo.pivotOffset = 1.7f;
		appInfo.gravityForce = 1.5f;
		break;
	case '3':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "mandelbulbFragment.c";
		appInfo.computeName = "mandelbulbCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = false;
		appInfo.explorationMode = false;

		appInfo.initialPosition = { 0.0f, 1.2f, 0.0f };
		appInfo.playerAcceleration = 0.1f;
		appInfo.collisionRadius = 0.04;
		appInfo.modelScale = { 0.05f, 0.05f, 0.05f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 1.0f;
		appInfo.pivotOffset = 0.7f;
		appInfo.gravityForce = 1.5f;
		break;
	case '4':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "mandelboxFragment.c";
		appInfo.computeName = "mandelboxCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = true;
		appInfo.explorationMode = true;

		appInfo.initialPosition = { 0.0f, -1.0f, 40.0f };
		appInfo.playerAcceleration = 10.0f;
		appInfo.collisionRadius = 0.065f;
		appInfo.modelScale = { 0.1f, 0.1f, 0.1f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 10.0f;
		appInfo.pivotOffset = 1.7f;
		appInfo.gravityForce = 1.5f;
		break;
	case '5':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "mandelboxTunnelFragment.c";
		appInfo.computeName = "mandelboxTunnelCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = true;
		appInfo.explorationMode = true;

		appInfo.initialPosition = { 0.0f, 0.0f, 5.5f };
		appInfo.playerAcceleration = 10.0f;
		appInfo.collisionRadius = 0.065f;
		appInfo.modelScale = { 0.1f, 0.1f, 0.1f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 0.25f;
		appInfo.pivotOffset = 1.7f;
		appInfo.gravityForce = 1.5f;
		break;
	case '6':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "planetAutumnFragment.c";
		appInfo.computeName = "planetAutumnCompute.c";
		appInfo.terrain = false;
		appInfo.freeCamera = false;
		appInfo.explorationMode = false;

		appInfo.initialPosition = { 0.0f, 1.0f, 0.0f };
		appInfo.playerAcceleration = 10.0f;
		appInfo.collisionRadius = 0.033f;
		appInfo.modelScale = { 0.05f, 0.05f, 0.05f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 1.0f;
		appInfo.pivotOffset = 0.7f;
		appInfo.gravityForce = 0.5f;
		break;
	case '7':
		appInfo.vertexName = "vertex.c";
		appInfo.fragmentName = "scene0fragment.c";
		appInfo.computeName = "scene0Compute.c";
		appInfo.terrain = true;
		appInfo.freeCamera = true;
		appInfo.explorationMode = true;

		appInfo.initialPosition = { 0.0f, 0.0f, 15.0f };
		appInfo.playerAcceleration = 50.0f;
		appInfo.collisionRadius = 2.7f;
		appInfo.modelScale = { 4.0f, 4.0f, 4.0f };
		appInfo.cameraDirection = { 0.0f, 0.0f, -1.0f };
		appInfo.cameraVelocity = 20.0f;
		appInfo.pivotOffset = 52.0f;
		appInfo.gravityForce = 1.5f;
		break;
	default:
		break;
	}

	return appInfo;
}

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
		else if (key == 'C') player.setRadius(player.getRadius() + scaleIncrease);
		else if (key == 'V') player.setRadius(player.getRadius() - scaleIncrease);
		else if (key == 'B') appInfo.modelScale += scaleIncrease;
		else if (key == 'N') appInfo.modelScale -= scaleIncrease;
		else if (key == 'H') debug = !debug;
		//DEBUG

		else if(key == 'W' || key == 'A' || key == 'S' || key == 'D') {
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
	camera.setEye(appInfo.initialPosition);
	camera.setFront(appInfo.cameraDirection);
	camera.setVelocity(appInfo.cameraVelocity);
	pivotOffset = appInfo.pivotOffset;

	std::list<RenderShader*> renderShaders;

	ComputeShader computeShader;
	if (!appInfo.explorationMode) {
		computeShader.init(compute);
		computeShader.use();
		appManager->addComputeShader(&computeShader);
	}

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
	player = PlayableSphere(gravityDirection, { 0.0f, 0.0f, 0.0f }, appInfo.initialPosition, appInfo.gravityForce, 1.5f, appInfo.playerAcceleration, 0.01f, 0.2f, appInfo.collisionRadius);

	computeShader.setSSBO(player.getSSBO());

	glm::mat4 unityMatrix = glm::mat4(1.0f);
	glm::mat4 model = unityMatrix;
	ubo.projection = glm::perspective(glm::radians(camera.getZoom()), (float)window->getWindowWidth() / (float)window->getWindowHeight(), 0.1f, 100.0f);
	if (appInfo.explorationMode)
		model = glm::translate(unityMatrix, { 0.0f, -100.0f, 0.0f });

	float lastCameraYaw = 0.0f;
	float lastDepthAdvance = 0.0f;
	float lastLateralAdvance = 0.0f;
	float rotationAngle = 300.0f;
	glm::vec3 lastPlayerPosition = { 0.0f, 0.0f, 0.0f };

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

		if(!appInfo.explorationMode)
			model = glm::translate(unityMatrix, lastPlayerPosition); // translate 

		model = glm::rotate(model, glm::radians(-lastCameraYaw - 180.0f), { 0, 1, 0 }); // rotate
		model = glm::rotate(model, glm::radians(lastDepthAdvance), { 0, 0, 1 });
		model = glm::rotate(model, glm::radians(lastLateralAdvance), { 1, 0, 0 });
		model = glm::scale(model, appInfo.modelScale);	// scale 

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
		std::cout << "CONTROLS" << std::endl;
		std::cout << "-Use W, A, S, D to move the camera or the player" << std::endl;
		std::cout << "-Use the mouse to rotate the camera or the player" << std::endl;
		std::cout << "-Use F to switch between free camera mode and player controller mode" << std::endl;
		std::cout << "-Use Z to reduce the camera pivot offset and X to increase it" << std::endl;
		std::cout << "-Use V to reduce the hitbox radius and C to increase it" << std::endl;
		std::cout << "-Use N to reduce the player radius and B to increase it" << std::endl;
		std::cout << "-Use H to view the hitbox in the debug mode" << std::endl;
		std::cout << std::endl;
		std::cout << "TERRAINS" << std::endl;
		std::cout << "Write the desired terrain" << std::endl;
		std::cout << "1: Snow terrain" << std::endl;
		std::cout << "2: Ocean terrain" << std::endl;
		std::cout << "3: Mandelbulb (experimental)" << std::endl;
		std::cout << "4: Mandelbox" << std::endl;
		std::cout << "5: Mandelbox tunnel" << std::endl;
		std::cout << "6: Autumn planet (experimental)" << std::endl;
		std::cout << "7: Primitives scene" << std::endl;
		std::cout << "Q: Exit application" << std::endl;
		std::cout << std::endl;
		std::cout << "Enter your selection: ";
		std::cin >> option;

		option = std::toupper(option);
		if (option == 'Q') {
			break;
		}

	} while(option <= '0' || option > '7');

	return option;
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