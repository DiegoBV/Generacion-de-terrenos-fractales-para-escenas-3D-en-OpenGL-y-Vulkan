#pragma once
#include <freeglut.h>
#include <glm.hpp>

class Viewport {
public:
	Viewport(GLsizei aw, GLsizei ah) : w(aw), h(ah) { };
	~Viewport() { };

	void setSize(GLsizei aw, GLsizei ah);
	void setPos(GLsizei ax, GLsizei ay);

	inline GLsizei getX() { return x; };
	inline GLsizei getY() { return y; };
	inline GLsizei getW() { return w; };
	inline GLsizei getH() { return h; };

protected:
	GLint x = 0, y = 0;
	GLsizei w, h;
	void set();
};
