#include "Camera.h"

void Camera::set(Point3 eye, Point3 look, Vector3 up) {
	this->eye.set(eye);
	this->look.set(look);
	n.set(eye.x - look.x, eye.y - look.y, eye.z - look.z); // make n
	u.set(up.cross(n)); // make u = up X n
	n.normalize(); u.normalize(); // make them unit length
	v.set(n.cross(u)); // make v = n X u
	setModelViewMatrix(); // tell OpenGL
}

void Camera::set_looking_same_point(Point3 eye, Vector3 up) {
	this->eye.set(eye);
	n.set(eye.x - this->look.x, eye.y - this->look.y, eye.z - this->look.z); // make n
	u.set(up.cross(n)); // make u = up X n
	n.normalize(); u.normalize(); // make them unit length
	v.set(n.cross(u)); // make v = n X u
	setModelViewMatrix(); // tell OpenGL
}

void Camera::slide(float delU, float delV, float delN)
{
	eye.x += delU * u.x + delV * v.x + delN * n.x;
	eye.y += delU * u.y + delV * v.y + delN * n.y;
	eye.z += delU * u.z + delV * v.z + delN * n.z;
	if (this->follow)
		set_looking_same_point(eye, Vector3(0, 0, 1));
	else
		setModelViewMatrix();
}

void Camera::roll(float angle)
{ // roll the camera through angle degrees
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);
	Vector3 t(u); // remember old u
	u.set(cs*t.x - sn*v.x, cs*t.y - sn*v.y, cs*t.z - sn*v.z);
	v.set(sn*t.x + cs*v.x, sn*t.y + cs*v.y, sn*t.z + cs*v.z);
	setModelViewMatrix();
}

void Camera::pitch(float angle)
{ // roll the camera through angle degrees
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);
	Vector3 t(v); // remember old u
	v.set(cs*t.x - sn*n.x, cs*t.y - sn*n.y, cs*t.z - sn*n.z);
	n.set(sn*t.x + cs*n.x, sn*t.y + cs*n.y, sn*t.z + cs*n.z);
	//n.normalize();
	//this->look.set(Point3(n.x, n.y, n.z));
	setModelViewMatrix();
}

void Camera::yaw(float angle)
{ // roll the camera through angle degrees
	float cs = cos(3.14159265 / 180 * angle);
	float sn = sin(3.14159265 / 180 * angle);
	Vector3 t(n); // remember old u
	n.set(cs*t.x - sn*u.x, cs*t.y - sn*u.y, cs*t.z - sn*u.z);
	u.set(sn*t.x + cs*u.x, sn*t.y + cs*u.y, sn*t.z + cs*u.z);
	//n.normalize();
	//this->look.set(Point3(n.x, n.y, n.z));
	setModelViewMatrix();
}

void Camera::setShape(float vAng, float asp, float nearD, float farD) {
	this->viewAngle = vAng;
	this->aspect = asp;
	if (nearD > 0)
		this->nearDist = nearD;
	if (farD > 0)
		this->farDist = farD;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW);
	this->projection_close2GLnew->setProjection(nearD, farD, asp, vAng);
	//glLoadIdentity();
}
