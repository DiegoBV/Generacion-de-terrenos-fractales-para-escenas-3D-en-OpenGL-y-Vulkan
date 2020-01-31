#include "Camera.h"

#include <gtc/matrix_transform.hpp>  
#include <gtc/type_ptr.hpp>

using namespace glm;

//-------------------------------------------------------------------------

 void Viewport::setSize(GLsizei aw, GLsizei ah) 
 { 
   w = aw; 
   h = ah; 
   set(); 
 }

 void Viewport::setPos(GLsizei ax, GLsizei ay)
 {
	x = ax;
	y = ay;
	set();
 }
 //-------------------------------------------------------------------------

 void Viewport::set() 
 { 
   glViewport(x, y, w, h);
 }
//-------------------------------------------------------------------------

void Camera::setAZ() 
{
  eye= dvec3(0, 0, -50);
  look= dvec3(0, 0, 0);
  up= dvec3(0, 1, 0);
  update();//actualizamos vectores, pitch y yaw
  p = 0;
  y = 0;
  viewMat = lookAt(eye, look, up);
  setVM();
}
//-------------------------------------------------------------------------

void Camera::set3D() 
{
  eye= dvec3(500, 500, 500);
  look= dvec3(0, 10, 0);
  up = dvec3(0, 1, 0);
  update();//actualizamos vectores, pitch y yaw
  p = degrees(asin(-front.y));
  y = degrees(asin(-front.x / cos(radians(p))));
  viewMat = lookAt(eye, look, up);
  setVM();
}
//-------------------------------------------------------------------------

void Camera::setVM() 
{
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixd(value_ptr(viewMat));
}
//-------------------------------------------------------------------------

void Camera::pitch(GLdouble a) 
{  
	rotatePY(a, 0);
}
//-------------------------------------------------------------------------
void Camera::yaw(GLdouble a)
{
	rotatePY(0, a);
}
//-------------------------------------------------------------------------
void Camera::roll(GLdouble a)
{
  //viewMat = rotate(viewMat, glm::radians(-a), glm::dvec3(0, 0, 1.0));
}
//-------------------------------------------------------------------------

void Camera::scale(GLdouble s)
{ 
  factScale -= s; 
  if (s < 0) s = 0.01;
  setPM(); 
}
//-------------------------------------------------------------------------

void Camera::setSize(GLdouble aw, GLdouble ah) 
{
  xRight = aw / 2.0;
  xLeft = -xRight;
  yTop = ah / 2.0;
  yBot = -yTop;
 
  setPM();
}
//-------------------------------------------------------------------------

void Camera::setPM() 
{
	if (orto){//si orto esta a true la vista sera ortogonal
		glMatrixMode(GL_PROJECTION);
		projMat = ortho(xLeft*factScale, xRight*factScale, yBot*factScale, yTop*factScale, nearVal, farVal);
		//(left, right, bottom, top, near, far)
		glLoadMatrixd(value_ptr(projMat));
		glMatrixMode(GL_MODELVIEW);
	}
	else {//si no, sera en perspectiva
		glMatrixMode(GL_PROJECTION);
		projMat = frustum(xLeft*factScale, xRight*factScale, yBot*factScale, yTop*factScale, 2*yTop, farVal);
		//near es 2*top para un fov de 60. Si se quiere un fov de 90 near = top
		glLoadMatrixd(value_ptr(projMat));
		glMatrixMode(GL_MODELVIEW);
	}
}

//-------------------------------------------MOVIMIENTO/ROTACION-------------------------------------------------------------------

//movimiento izq/der, utilizaremos el vector right (= u)
void Camera::moveLR(GLdouble cs) {
	eye += right * cs;//movimiento sobre el sistema de la camara (no global)
	look += right * cs;//modificamos look tambien para no modificar la direccion de vista 
	//(si no siempre se queda mirando al mismo punto, como si pivotase)
	viewMat = lookAt(eye, look, up);
	update();//actualizamos los vectores
}

//lo mismo con adelante/atras, utilizaremos el vector front (= n)
void Camera::moveFB(GLdouble cs) {
	eye += (-front) * cs;//-front porque n apunta a la direccion opuesta a la que miramos
	look += (-front) * cs;
	viewMat = lookAt(eye, look, up);
	update();//actualizamos los vectores
}

//lo mismo con arriba/abajo, utilizaremos el vector up
void Camera::moveUD(GLdouble cs) {
	eye += up * cs;
	look += up * cs;
	viewMat = lookAt(eye, look, up);
	//no necesita actualizar vectores, siempre va hacia arriba o hacia abajo, no depende de la rotacion
}

//rota en el eje correspondiente
void Camera::rotatePY(GLdouble incrPitch, GLdouble incrYaw) {
	p += incrPitch; 
	y += incrYaw; // Actualizar los ángulos
	if (p > 89.5) p = 89.5; // Limitar los ángulos
	else if (p < -89.5) p = -89.5; // impide que des la vuelta en el eje y (si miras hacia arriba o abajo)
	// Actualizar la dirección de vista
	front.x = sin(radians(y)) * cos(radians(p));
	front.y = sin(radians(p));
	front.z = -cos(radians(y)) * cos(radians(p));
	front = glm::normalize(front);
	look = eye + front;//actualizamos look; si no, se queda con el look anterior y la camara se reinicia a esa posicion
	viewMat = lookAt(eye, look, up);
}
//---------------------------------------------------------------------------------------------------------------------------------

//activa/desactiva la vista en perspectiva, es llamado al pulsar "p" (main.cpp)
void Camera::setPrj(){
	orto = !orto;//bool que alterna entre orotgonal (true) y perspectiva (false)
	setPM();
}
//-------------------------------------------------------------------------


