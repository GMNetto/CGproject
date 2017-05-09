#pragma once

#include <glut.h>
#include <GL/glu.h>	
#include <math.h>
#include "Util.h"
#include "Matrix.h"
#include <vector>

class Camera
{
public:
	Camera(Matrix *viewModel_close2GL_, Matrix *projection_close2GLnew_, Matrix *viewPort_close2GL_, int *windows_id, size_t number) {
		u = { 1, 0, 0 };
		u = { 0, 1, 0 };
		n = { 0, 0, -1 };
		this->viewModel_close2GL = viewModel_close2GL_;
		this->projection_close2GLnew = projection_close2GLnew_;
		this->viewPort_close2GL = viewPort_close2GL_;
		windows = windows_id;
		size_w = number;
	};
	void set(Point3 eye, Point3 look, Vector3 up);
	void set_looking_same_point(Point3 eye, Vector3 up);
	void roll(float angle);
	void pitch(float angle);
	void yaw(float angle);
	void slide(float delU, float delV, float delN);
	void setShape(float vAng, float asp, float nearD, float farD);
	float getNearDist() {
		return this->nearDist;
	};
	float getFarDist() {
		return this->farDist;
	};
	void set_follow() {
		this->follow = (this->follow + 1) % 2;
	}
	Vector3 get_coord_u() {
		return u;
	}

	Vector3 get_coord_v() {
		return v;
	}
	Vector3 get_coord_n() {
		return n;
	}

	Point3 get_eye() {
		return eye;
	}

	void setModelViewMatrix();

	

private:
	int follow = 0;
	Point3 eye, look;
	Vector3 u, v, n;
	int hfov = 60, vfov = 60;
	float viewAngle, aspect, nearDist, farDist;
	Matrix *viewModel_close2GL, *projection_close2GLnew, *viewPort_close2GL;
	int *windows, size_w = 0;
};


inline void Camera::setModelViewMatrix()
{
	float m[16];
	Vector3 eVec{ eye.x, eye.y, eye.z };
	m[0] = u.x; m[4] = u.y; m[8] = u.z; m[12] = -eVec.dot(u);
	m[1] = v.x; m[5] = v.y; m[9] = v.z; m[13] = -eVec.dot(v);
	m[2] = n.x; m[6] = n.y; m[10] = n.z; m[14] = -eVec.dot(n);
	m[3] = 0; m[7] = 0; m[11] = 0; m[15] = 1.0;
	this->viewModel_close2GL->setModelView(this->u, this->v, this->n, eVec);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLoadMatrixf(m); // load OpenGL’s modelview matrix

	/*for (int i = 0; i < size_w; i++)
	{
		glutSetWindow(windows[i]);
		glutPostRedisplay();
	}*/
	//glutSetWindow(windows[0]);
}


