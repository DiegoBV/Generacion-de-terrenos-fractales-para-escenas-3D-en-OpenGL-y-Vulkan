#include "Viewport.h"

using namespace glm;

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

void Viewport::set()
{
	glViewport(x, y, w, h);
}