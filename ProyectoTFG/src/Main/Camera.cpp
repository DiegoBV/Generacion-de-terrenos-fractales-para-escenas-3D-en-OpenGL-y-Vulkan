//#include "Camera.h"
//
//#include <gtc/matrix_transform.hpp>  
//#include <gtc/type_ptr.hpp>
//#include <gtc/matrix_access.hpp>
//
//using namespace glm;
//
////-------------------------------------------------------------------------
//
//void Camera::uploadVM() const
//{
//	glMatrixMode(GL_MODELVIEW);
//	glLoadMatrixd(value_ptr(viewMat));
//}
////-------------------------------------------------------------------------
//
//void Camera::setAxes()
//{
//	right = row(viewMat, 0);
//	upward = row(viewMat, 1);
//	front = -row(viewMat, 2);
//}
////-------------------------------------------------------------------------
//void Camera::setVM()
//{
//	viewMat = lookAt(eye, look, up);
//	setAxes();
//}
////-------------------------------------------------------------------------
//
//void Camera::set2D()
//{
//	eye = dvec3(0, 0, radio);
//	look = dvec3(0, 0, 0);
//	up = dvec3(0, 1, 0);
//	setVM();
//	ang = -90;
//}
////-------------------------------------------------------------------------
//
//void Camera::set3D()
//{
//	//eye = dvec3(radio, radio, radio);  // lo recoloca orbit
//	GLdouble aux = cos(radians(-45.0)) * radio;
//	eye = dvec3(aux, aux, aux);
//	look = dvec3(0, 0, 0);   //dvec3(0, 10, 0);
//	up = dvec3(0, 1, 0);
//	setVM();
//	ang = -45;
//}
////-------------------------------------------------------------------------
//
//void Camera::setCenital()
//{
//	eye = dvec3(0, radio, 0);
//	look = dvec3(0, 0, 0);
//	up = dvec3(0, 0, -1);
//	setVM();
//	ang = -90;
//}
////-------------------------------------------------------------------------
//
//void Camera::moveLR(GLdouble cs)
//{
//	eye += right * cs; // *cameraSpeed
//	look += right * cs;
//	setVM();
//}
////-------------------------------------------------------------------------
//
//void Camera::moveUD(GLdouble cs)
//{
//	eye += upward * cs;  // *cameraSpeed
//	look += upward * cs;
//	setVM();
//}
////-------------------------------------------------------------------------
//
//void Camera::moveFB(GLdouble cs)
//{
//	eye += front * cs;  // *cameraSpeed
//	look += front * cs;
//	setVM();
//}
////-------------------------------------------------------------------------
//
//void Camera::orbit(GLdouble ax, GLdouble ay)
//{
//	ang += ax;
//	eye.x = look.x + cos(radians(ang)) * radio;
//	eye.z = look.z - sin(radians(ang)) * radio;
//	eye.y += ay;
//	setVM();
//}
////-------------------------------------------------------------------------
//
//void Camera::lookLR(GLdouble cs)
//{
//	look += right * cs;
//	setVM();
//}
////-------------------------------------------------------------------------
//
//void Camera::lookUD(GLdouble cs)
//{
//	look += upward * cs;
//	setVM();
//}
//
////-------------------------------------------------------------------------
///*
//void Camera::pitch(GLdouble a)
//{
//	viewMat = rotate(viewMat, glm::radians(-a), glm::dvec3(1.0, 0, 0));
//}
////-------------------------------------------------------------------------
//void Camera::yaw(GLdouble a)
//{
//	viewMat = rotate(viewMat, glm::radians(-a), glm::dvec3(0, 1.0, 0));
//}
////-------------------------------------------------------------------------
//void Camera::roll(GLdouble a)
//{
//	viewMat = rotate(viewMat, glm::radians(-a), glm::dvec3(0, 0, 1.0));
//}
//*/
//
////-------------------------------------------------------------------------
//
//void Camera::setSize(GLdouble aw, GLdouble ah)
//{
//	xRight = aw / 2.0;
//	xLeft = -xRight;
//	yTop = ah / 2.0;
//	yBot = -yTop;
//	setPM();
//}
////-------------------------------------------------------------------------
//
//void Camera::setScale(GLdouble s)
//{
//	factScale -= s;
//	if (factScale < 0) factScale = 0.01;
//	setPM();
//}
////-------------------------------------------------------------------------
//void Camera::setPM()
//{
//	if (orto)
//	{
//		nearVal = 1;
//		projMat = ortho(xLeft * factScale, xRight * factScale, yBot * factScale, yTop * factScale, nearVal, farVal);
//	}
//	else
//	{
//		nearVal = 2 * yTop; // *factScale;
//		projMat = frustum(xLeft * factScale, xRight * factScale, yBot * factScale, yTop * factScale, nearVal, farVal);
//	}
//}
//
//void Camera::uploadPM() const
//{
//	glMatrixMode(GL_PROJECTION);
//	glLoadMatrixd(value_ptr(projMat));
//	glMatrixMode(GL_MODELVIEW);
//}
////-------------------------------------------------------------------------
//
//
