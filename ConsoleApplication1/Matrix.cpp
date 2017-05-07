#include "Matrix.h"
#include "math.h"

Matrix::Matrix(const float data[4][4]) {
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			this->data[i][j] = data[i][j];
		}
	}
}

void Matrix::multiply(const Matrix& matrix) {
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t k = 0; k < 4; k++)
		{
			float accum = 0;
			for (size_t j = 0; j < 4; j++)
			{
				accum += this->data[i][j] * matrix.data[j][i];
			}
			this->data[i][k] = accum;
		}
	}
}

Matrix Matrix::multiply_new(const Matrix& matrix) {
	float new_data[4][4];
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t k = 0; k < 4; k++)
		{
			float accum = 0;
			for (size_t j = 0; j < 4; j++)
			{
				accum += this->data[i][j] * matrix.data[j][i];
			}
			new_data[i][k] = accum;
		}
	}
	return Matrix(new_data);
}

Matrix Matrix::vector2matrix(const Vector3& vector) {
	float data[4][4];
	float vector_4[4]{vector.x, vector.y, vector.z, 1};
	for (size_t i = 0; i < 4; i++)
	{
		float accum = 0;
		for (size_t j = 0; j < 4; j++)
		{
			data[i][j] = 0;
			accum += this->data[i][j] * vector_4[j];
		}
		data[i][0] = accum;
	}
	return Matrix(data);
}

void Matrix::setModelView(Vector3& u, Vector3& v, Vector3& n, Vector3& eye) {
	this->data[0][0] = u.x;
	this->data[0][1] = u.y;
	this->data[0][2] = u.z;
	this->data[0][3] = -u.dot(eye);
	this->data[1][0] = v.x;
	this->data[1][1] = v.y;
	this->data[1][2] = v.z;
	this->data[1][3] = -v.dot(eye);
	this->data[2][0] = n.x;
	this->data[2][1] = n.y;
	this->data[2][2] = n.z;
	this->data[2][3] = -n.dot(eye);
	this->data[3][0] = 0;
	this->data[3][1] = 0;
	this->data[3][2] = 0;
	this->data[3][3] = 1;
}

void Matrix::setProjection(float near_d, float far_d, float aspect, float fovy) {
	/*this->data[0][0] = 2*near_d/(right*left);
	this->data[0][1] = 0;
	this->data[0][2] = (right + left)/(right - left);
	this->data[0][3] = 0;
	this->data[1][0] = 0;
	this->data[1][1] = 2*near_d/(top - bottom);
	this->data[1][2] = (top + bottom)/ (top - bottom);
	this->data[1][3] = 0;*/
	float f = cos(fovy / 2) / sin(fovy / 2);
	this->data[0][0] = 2*f/aspect;
	this->data[0][1] = 0;
	this->data[0][2] = 0;
	this->data[0][3] = 0;
	this->data[1][0] = 0;
	this->data[1][1] = f;
	this->data[1][2] = 0;
	this->data[1][3] = 0;
	this->data[2][0] = 0;
	this->data[2][1] = 0;
	this->data[2][2] = (far_d + near_d) / (far_d - near_d);
	this->data[2][3] = 2 * far_d*near_d / (far_d - near_d);
	this->data[3][0] = 0;
	this->data[3][1] = 0;
	this->data[3][2] = -1;
	this->data[3][3] = 0;
}

void Matrix::setViewPort(float rv, float tv, float lv, float bv) {
	this->data[0][0] = (rv - lv) / 2;
	this->data[0][1] = 0;
	this->data[0][2] = 0;
	this->data[0][3] = (rv + lv) / 2;
	this->data[1][0] = 0;
	this->data[1][1] = (tv - bv)/2;
	this->data[1][2] = 0;
	this->data[1][3] = (tv + bv) / (2);
	this->data[2][0] = 0;
	this->data[2][1] = 0;
	this->data[2][2] = 1;
	this->data[2][3] = 0;
	this->data[3][0] = 0;
	this->data[3][1] = 0;
	this->data[3][2] = 0;
	this->data[3][3] = 1;
}