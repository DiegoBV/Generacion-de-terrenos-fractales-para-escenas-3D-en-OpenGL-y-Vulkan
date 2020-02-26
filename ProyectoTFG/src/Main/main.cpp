#include <iostream>
#include <vector>
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"
#include "Shader.h"

Camera camera;
glm::dvec2 mCoord;

//calcula el desplazamiento del raton al moverlo y manda a la camara moverse en funcion de el
//callback llamado cada vez que movemos el raton. Solo manda mover a la camara cuando estamos clicando, 
//momento en el cual se actualiza mCoord (se llama a mouse) y se produce un desplazamiento
void motion(GLFWwindow* window, double xpos, double ypos) {
	glm::dvec2 mOffset = mCoord; // var. global (mCoord vale las coordenadas del raton)
	mCoord = glm::dvec2(xpos, ApplicationManager::GetSingleton()->getWindowHeight() - ypos);//actualizamos mCoord invirtiendo el eje y
	mOffset = (mCoord - mOffset) * 0.05; // sensitivity. mOffset vale la diferencia (desplazamiento -> nuevas coord del raton - coord anteriores)
	camera.handleOrientation(mOffset.x, mOffset.y);//mandamos que se mueva con el desplazamiento
}

//actualiza las coordenadas del raton invirtiendo el eje y; callback llamado cada vez que clicamos
void mouse(GLFWwindow* window, double xpos, double ypos) {
	mCoord = glm::dvec2(xpos, ApplicationManager::GetSingleton()->getWindowHeight() - ypos);
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

	std::vector<Manager*> managers;
	managers.push_back(timeManager);
	managers.push_back(appManager);

	for (Manager* manager : managers) manager->init();

	Shader shader = Shader();
	shader.init("..\\..\\Shaders\\vertex.c", "..\\..\\Shaders\\fragment.c");
	shader.use();
	shader.setVec2("resolution", appManager->getWindowWidth(), appManager->getWindowHeight());
	appManager->setShader(&shader);

	// Callback registration
	appManager->setKeyCallback(key);
	appManager->setCursorCallback(motion);
	//static_cast<VulkanManager*>(appManager)->camera = &camera;

	// render loop
	// -----------
	while (!appManager->shouldClose())
	{
		for (Manager* manager : managers) manager->update();

		shader.setVec3("cameraEye", camera.getEye().x, camera.getEye().y, camera.getEye().z);
		shader.setVec3("cameraFront", camera.getFront().x, camera.getFront().y, camera.getFront().z);
		shader.setVec3("worldUp", camera.getWorldUp().x, camera.getWorldUp().y, camera.getWorldUp().z);
		shader.setMat4("viewMat", transpose(camera.getViewMatrix()));
		shader.setFloat("time", timeManager->getTimeSinceBeginning());
		shader.use();
	}

	appManager->waitUntilFinishEverything();
	appManager->ShutDownSingleton();
	timeManager->ShutDownSingleton();
	return 0;
}