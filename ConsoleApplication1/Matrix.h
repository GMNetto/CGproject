#pragma once
#include <algorithm>
#include "Util.h"

class Matrix {
public:
	Matrix(const float data[4][4]);

	void multiply(const Matrix& matrix);

	Matrix multiply_new(const Matrix& matrix);

	Matrix vector2matrix(const Vector3& vector);

	void setModelView(Vector3& u, Vector3& v, Vector3& n, Vector3& eye);

	void setProjection(float near_d, float far_d, float aspect, float fovy);

	void setViewPort(float width, float height);

	float get_data(int i, int j) {
		return data[i][j];
	}

	void set_data(int i, int j, float value) {
		data[i][j] = value;
	}

	void transpose() {
		for (size_t i = 0; i < 4; i++)
		{
			for (size_t j = i+1; j < 4; j++)
			{
				float temp = this->data[i][j];
				this->data[i][j] = this->data[j][i];
				this->data[j][i] = temp;
			}
		}
	}

	void normalize_vertex_project() {
		this->set_data(0, 0, this->get_data(0, 0) / this->get_data(3, 0));
		this->set_data(1, 0, this->get_data(1, 0) / this->get_data(3, 0));
		this->set_data(2, 0, this->get_data(2, 0) / this->get_data(3, 0));
		this->set_data(3, 0, this->get_data(3, 0) / this->get_data(3, 0));
	}

	bool check_vertex_projected() {
		return (abs(get_data(0, 0)) > abs(get_data(3, 0))
			|| abs(get_data(1, 0)) > abs(get_data(3, 0))
			|| abs(get_data(2, 0)) > abs(get_data(3, 0)));
	}

private:
	float data[4][4];

};
