#pragma once

#include <freeglut.h>
#include <glm.hpp>

class Viewport;

class Camera {
public:
	Camera();
	Camera(Viewport* avp);
	~Camera() {};

	inline Viewport* getVP() { return vp; }
	inline glm::dmat4 const& getViewMat() { return viewMat; };
	inline glm::dmat4 const& getProjMat() { return projMat; };
	inline glm::dvec3 getEye() { return eye; }
	inline glm::dvec3 getFront() { return front; }

	void setAZ();  // lookAt(eye(0,0,500), look(0,0,0) up(0, 1, 0))
	void setSize(GLdouble aw, GLdouble ah);
	void scale(GLdouble s);

	void pitch(GLdouble a); // rotates a degrees on the X axis
	void yaw(GLdouble a);   // rotates a degrees on the Y axis
	void roll(GLdouble a);  // rotates a degrees on the Z axis

	void moveLR(GLdouble cs); // Left / Right
	void moveFB(GLdouble cs); // Forward / Backward
	void moveUD(GLdouble cs); // Up / Down
	void rotatePY(GLdouble incrPitch, GLdouble incrYaw);//rota en el eje correspondiente

protected:
	glm::dvec3 eye = { 0.0, 0.0, 500.0 };
	glm::dvec3 look = { 0.0, 0.0, 0.0 };

	//vectores de direccion de la camara
	glm::dvec3 up = { 0.0, 1.0, 0.0 };
	glm::dvec3 front = { 0.0, 0.0, 1.0 };
	glm::dvec3 right = { 1.0, 0.0, 0.0 };

	glm::dmat4 viewMat;  // inverse

	GLdouble xRight, xLeft, yTop, yBot;
	GLdouble nearVal = 1;
	GLdouble p = 0, y = 0;//angulos de rotacion (x, y, respectivamente)
	GLdouble farVal = 10000;
	GLdouble factScale = 1;
	glm::dmat4 projMat;

	Viewport* vp;

	void setVM();
	void setPM();

	//actualiza los vectores de direccion de la camara
	void update();
};