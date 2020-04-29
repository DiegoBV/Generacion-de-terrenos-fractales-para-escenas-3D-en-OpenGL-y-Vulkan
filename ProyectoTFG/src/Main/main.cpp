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
		//DEBUG
		if (key == 'Z') pivotOffset -= 0.1f;
		else if (key == 'X') pivotOffset += 0.1f;
		else if (key == 'C') player.setRadius(player.getRadius()+0.005f);
		else if (key == 'V') player.setRadius(player.getRadius()-0.005f);
		//DEBUG
		else player.handleMovement(key, camera.getFront());
	}

	//double deltaTime = TimeManager::GetSingleton()->getDeltaTime();

	//switch (key) {
	//case 'W'://si pulsamos "w" acercamos la camara
	//	camera.handleMovement(Camera_Movement::FORWARD, deltaTime);
	//	break;
	//case 'S'://"s" la alejamos
	//	camera.handleMovement(Camera_Movement::BACKWARD, deltaTime);
	//	break;
	//case 'D'://derecha
	//	camera.handleMovement(Camera_Movement::RIGHT, deltaTime);
	//	break;
	//case 'A'://izquierda
	//	camera.hand leMovement(Camera_Movement::LEFT, deltaTime);
	//	break;
	//default:
	//	break;
	//}//switch
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

void runApplication(const std::string& vertex, const std::string& fragment) {

	TimeManager* timeManager = TimeManager::GetSingleton();
	ApplicationManager* appManager = ApplicationManager::GetSingleton();
	Window* window = Window::GetSingleton();

	std::list<RenderShader*> renderShaders;

	ComputeShader computeShader = ComputeShader();
	computeShader.init("compute.c");
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
	appManager->addRenderShader(&modelShader);
	renderShaders.push_back(&modelShader);

	appManager->GetSingleton()->setUpGraphicsPipeline();

	// Callback registration
	window->setKeyCallback(key);
	window->setCursorCallback(motion);

	UniformBufferObject ubo;
	ubo.resolution = { window->getWindowWidth(), window->getWindowHeight() };
	ubo.worldUp = camera.getWorldUp();
	ubo.playerColor = glm::vec4(1.0, 0.0, 0.0, 1.0);

	player = PlayableSphere({ 0.0f, -1.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.75f, 50.0f }, 1.5f, 5.0f, 0.1f, 0.01f);

	computeShader.setSSBO(player.getSSBO());


	glm::mat4 unityMatrix = glm::mat4(1.0f);
	glm::mat4 model = unityMatrix;
	ubo.projection = glm::perspective(glm::radians(camera.getZoom()), (float)window->getWindowWidth() / (float)window->getWindowHeight(), 0.1f, 100.0f);
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

		model = glm::translate(unityMatrix, ubo.playerPos); // translate it down so it's at the center of the scene
		model = glm::rotate(model, glm::radians(-(camera.getYaw() + 90.0f)), { 0, 1, 0 });
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
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

std::pair<const std::string, const std::string> selectShaders(const char& option) {
	std::pair<std::string, std::string> shaders;

	switch (option)
	{
	case '1':
		shaders.first = "vertex.c";
		shaders.second = "snowTerrainFragment.c";
		break;
	case '2':
		shaders.first = "vertex.c";
		shaders.second = "mandelbulbFragment.c";
		break;
	case '3':
		shaders.first = "vertex.c";
		shaders.second = "mandelboxFragment.c";
		break;
	case '4':
		shaders.first = "vertex.c";
		shaders.second = "scene0fragment.c";
		break;
	default:
		break;
	}

	return shaders;
}

int main()
{
	char selectedOption = menu();
	while (selectedOption != 'Q')
	{
		std::pair<const std::string, const std::string> shaders = selectShaders(selectedOption);
		init();
		runApplication(shaders.first, shaders.second);
		release();

		selectedOption = menu();
	}

	return 0;
}