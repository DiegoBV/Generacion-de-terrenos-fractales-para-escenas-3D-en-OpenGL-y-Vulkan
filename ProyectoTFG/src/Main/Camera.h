//#pragma once
#ifndef _H_Camera_H_
#define _H_Camera_H_

#include <freeglut.h>
#include <glm.hpp>

//-------------------------------------------------------------------------

class Viewport {   
public:
  Viewport(GLsizei aw, GLsizei ah) : w(aw), h(ah) { };
  ~Viewport() { };

  void setSize(GLsizei aw, GLsizei ah);
  void setPos(GLsizei ax, GLsizei ay);
 
  GLsizei getX() { return x; };
  GLsizei getY() { return y; };
  GLsizei getW() { return w; };
  GLsizei getH() { return h; };

protected:
  GLint x=0, y=0;
  GLsizei w, h;  
  void set();
};

//-------------------------------------------------------------------------

class Camera {
public:
    Camera() {}
	Camera(Viewport* avp) : vp(avp), viewMat(1.0), projMat(1.0),
		xRight(avp->getW() / 2.0), xLeft(-xRight), yTop(avp->getH() / 2.0), yBot(-yTop) {};
  ~Camera() {};
  Viewport* getVP() { return vp; }

  // view matrix
  glm::dmat4 const& getViewMat() { return viewMat; };

  void setAZ();  // lookAt(eye(0,0,500), look(0,0,0) up(0, 1, 0))
  void set3D();  // lookAt(eye(500,500,500), look(0,10,0) up(0, 1, 0))
    
  void pitch(GLdouble a); // rotates a degrees on the X axis
  void yaw(GLdouble a);   // rotates a degrees on the Y axis
  void roll(GLdouble a);  // rotates a degrees on the Z axis

  // projection matrix
  glm::dmat4 const& getProjMat() { return projMat; };

  void setSize(GLdouble aw, GLdouble ah);
  void scale(GLdouble s); 

  void moveLR(GLdouble cs); // Left / Right
  void moveFB(GLdouble cs); // Forward / Backward
  void moveUD(GLdouble cs); // Up / Down
  void rotatePY(GLdouble incrPitch, GLdouble incrYaw);//rota en el eje correspondiente
  void setPrj();//activa/desactiva la vista en perspectiva
  glm::dvec3 getEye(){ return eye; }
  glm::dvec3 getFront(){ return front; }
 
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
  bool orto = true;//bool que alterna entre orotgonal (true) y perspectiva (false)

  Viewport* vp;

  void setVM();
  void setPM();

  //actualiza los vectores de direccion de la camara
  void update() {
	  front = normalize(eye - look);
	  right = normalize(cross(up, front));
	  //up no es necesario, puesto que siempre va hacia arriba o hacia abajo, no depende de la rotacion
  }
};

//-------------------------------------------------------------------------

#endif //_H_Camera_H_