#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "Shader.h"
#include "Camera.h"
#include "Viewport.h"
#include "TimeManager.h"
#include "GLManager.h"

Camera camera;
glm::dvec2 mCoord;

//calcula el desplazamiento del raton al moverlo y manda a la camara moverse en funcion de el
//callback llamado cada vez que movemos el raton. Solo manda mover a la camara cuando estamos clicando, 
//momento en el cual se actualiza mCoord (se llama a mouse) y se produce un desplazamiento
void motion(GLFWwindow* window, double xpos, double ypos) {
	glm::dvec2 mOffset = mCoord; // var. global (mCoord vale las coordenadas del raton)
	mCoord = glm::dvec2(xpos, GLManager::GetSingleton()->getWindowHeight() - ypos);//actualizamos mCoord invirtiendo el eje y
	mOffset = (mCoord - mOffset) * 0.05; // sensitivity. mOffset vale la diferencia (desplazamiento -> nuevas coord del raton - coord anteriores)
	camera.ProcessMouseMovement(mOffset.x, mOffset.y);//mandamos que se mueva con el desplazamiento
}

//actualiza las coordenadas del raton invirtiendo el eje y; callback llamado cada vez que clicamos
void mouse(GLFWwindow* window, double xpos, double ypos) {
	mCoord = glm::dvec2(xpos, GLManager::GetSingleton()->getWindowHeight() - ypos);
}

void key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	double deltaTime = TimeManager::GetSingleton()->getDeltaTime();

	switch (key) {
	case 'W'://si pulsamos "w" acercamos la camara
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
		break;
	case 'S'://"s" la alejamos
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
		break;
	case 'D'://derecha
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
		break;
	case 'A'://izquierda
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
		break;
	default:
		break;
	}//switch
}

int main()
{
	TimeManager* timeManager = TimeManager::GetSingleton();
	GLManager* glManager = GLManager::GetSingleton();

	std::vector<Manager*> managers;
	managers.push_back(timeManager);
	managers.push_back(glManager);

	for (Manager* manager : managers) manager->init();

	//CAMERA
	// Viewport position and size
	Viewport viewPort(glManager->getWindowWidth(), glManager->getWindowHeight());
	// Camera position, view volume and projection
	//camera.setAZ();

	// Callback registration
	glfwSetKeyCallback(glManager->getWindow(), key);
	glfwSetInputMode(glManager->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(glManager->getWindow(), motion);

	Shader shader = Shader();
	shader.init("..\\Shaders\\vertex.c", "..\\Shaders\\fragment.c");
	shader.use();
	shader.setVec2("resolution", glManager->getWindowWidth(), glManager->getWindowHeight());

	// render loop
	// -----------
	while (!glfwWindowShouldClose(glManager->getWindow()))
	{
		for (Manager* manager : managers) manager->update();

		shader.setVec3("cameraPosition", camera.Position.x, camera.Position.y, camera.Position.z);
		shader.setVec3("cameraDirection", -camera.Front.x, camera.Front.y, camera.Front.z);
		shader.setFloat("time", timeManager->getTimeSinceBeginning());
		shader.use();

		// pass projection matrix to shader (note that in this case it could change every frame)
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)glManager->getWindowWidth() / (float)glManager->getWindowHeight(), 0.1f, 100.0f);
		shader.setMat4("projection", projection);

		// camera/view transformation
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
	}

	glManager->ShutDownSingleton();
	timeManager->ShutDownSingleton();

	return 0;
}