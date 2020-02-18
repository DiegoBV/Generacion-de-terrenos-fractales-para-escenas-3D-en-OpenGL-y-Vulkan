#include <iostream>
#include <vector>
#include "Shader.h"
#include "Camera.h"
#include "TimeManager.h"
#include "ApplicationManager.h"

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

	// Callback registration
	appManager->setKeyCallback(key);
	appManager->setCursorCallback(motion);

	/*Shader shader = Shader();
	shader.init("..\\..\\Shaders\\vertex.c", "..\\..\\Shaders\\fragment.c");
	shader.use();
	shader.setVec2("resolution", appManager->getWindowWidth(), appManager->getWindowHeight());*/

	// render loop
	// -----------
	while (!appManager->shouldClose())
	{
		for (Manager* manager : managers) manager->update();

		/*shader.setVec3("cameraEye", camera.getEye().x, camera.getEye().y, camera.getEye().z);
		shader.setVec3("cameraFront", camera.getFront().x, camera.getFront().y, camera.getFront().z);
		shader.setVec3("worldUp", camera.getWorldUp().x, camera.getWorldUp().y, camera.getWorldUp().z);
		shader.setMat4("viewMat", transpose(camera.getViewMatrix()));
		shader.setFloat("time", timeManager->getTimeSinceBeginning());
		shader.use();*/

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)appManager->getWindowWidth() / (float)appManager->getWindowHeight(), 0.1f, 100.0f);
		//shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.getViewMatrix();
		//shader.setMat4("view", view);
	}

	appManager->ShutDownSingleton();
	timeManager->ShutDownSingleton();

	return 0;
}