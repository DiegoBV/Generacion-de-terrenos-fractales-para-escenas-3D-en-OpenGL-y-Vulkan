#include <iostream>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"
#include "Window.h"
#include <math.h>
#include <vector>

Camera camera;
glm::dvec2 mCoord;
StorageBufferObject ssbo;
float velocity = 3.0f; //TODO: llevar a sus clases personales
float pivotOffset = 2.0f;

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
			ssbo.velocity = velocity;
			ssbo.direction.z = camera.getFront().z;
			ssbo.direction.x = camera.getFront().x;
			ssbo.direction.y = 0.0f;
			ssbo.direction = normalize(ssbo.direction);
			break;
		case 'S'://"s" la alejamos
			ssbo.velocity = velocity;
			ssbo.direction.z = -camera.getFront().z;
			ssbo.direction.x = -camera.getFront().x;
			ssbo.direction.y = 0.0f;
			ssbo.direction = normalize(ssbo.direction);
			break;
		case 'D'://derecha
			ssbo.velocity = velocity;
			ssbo.direction.x = -camera.getFront().z;
			ssbo.direction.z = camera.getFront().x;
			ssbo.direction.y = 0.0f;
			ssbo.direction = normalize(ssbo.direction);
			break;
		case 'A'://izquierda
			ssbo.velocity = velocity;
			ssbo.direction.x = camera.getFront().z;
			ssbo.direction.z = -camera.getFront().x;
			ssbo.direction.y = 0.0f;
			ssbo.direction = normalize(ssbo.direction);
			break;
		case 'Q':
			ssbo.velocity = velocity;
			ssbo.direction.y = 1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.x = 0.0f;
			break;
		case 'E':
			ssbo.velocity = velocity;
			ssbo.direction.y = -1.0f;
			ssbo.direction.z = 0.0f;
			ssbo.direction.x = 0.0f;
			break;
		default:
			break;
		}//switch
	}
	else if (state == GLFW_RELEASE)
		ssbo.velocity = 0.0f;
}

// VECTORES DIRECTORES PARA LAS COLISIONES
std::vector<glm::vec3> fibonacci_sphere() {
	float rnd = ((float)rand() / (RAND_MAX)) + 1;

	std::vector<glm::vec3> dirs;
	float offset = 2.0 / COLLISION_SAMPLES;

	float increment = M_PI * (3.0 - sqrt(5.0));

	for (int i = 0; i < COLLISION_SAMPLES; i++) {
		float y = ((i * offset) - 1) + (offset / 2);
		float r = sqrt(1 - pow(y, 2));

		float phi = (int(i + rnd) % COLLISION_SAMPLES) * increment;

		float x = cos(phi) * r;
		float z = sin(phi) * r;

		dirs.push_back(glm::normalize(glm::vec3(x, y, z)));
	}

	return dirs;
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

	ssbo.velocity = 0.0f;
	ssbo.radius = 0.05f;
	ssbo.gravity = { 0.0f, -0.8f, 0.0f };
	ssbo.position = { 0.0f, 0.75f, 50.0f };
	ssbo.direction = { 0.0f, 0.0f, -1.0f };
	std::vector<glm::vec3> dirs = fibonacci_sphere();
	std::copy(dirs.begin(), dirs.end(), ssbo.collisionDirs);

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

		camera.pivotTarget(ssbo.position, pivotOffset);

		ubo.cameraEye = camera.getEye();
		ubo.cameraFront = camera.getFront();
		ubo.viewMat = transpose(camera.getViewMatrix());
		ubo.time = timeManager->getTimeSinceBeginning();
		ubo.playerPos = ssbo.position;
		ubo.playerRadius = ssbo.radius;

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