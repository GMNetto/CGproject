#pragma once

#include <math.h>

typedef struct vector_3d {
	float x, y, z;

	vector_3d() {}

	vector_3d(float x, float y, float z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

}vector_3d;

typedef struct segment {
	vector_3d pos, normal;
	int color_index;
}Segment;

typedef struct face {
	segment v0, v1, v2;
	vector_3d face_normal;
	vector_3d diffuse_color;
}face;

typedef struct Point3 {
	float x, y, z;

	Point3() {};

	Point3(float x, float y, float z) : x(x), y(y), z(z) {};

	void set(const Point3 &point) {
		x = point.x;
		y = point.y;
		z = point.z;
	}

}Point3;

typedef struct vector3 {
	float x, y, z;

	vector3() {};

	vector3(float x, float y, float z) : x(x), y(y), z(z) {};


	float dot(const vector3 &vec) {
		return x*vec.x + y*vec.y + z*vec.z;
	}

	vector3 cross(const vector3 &vec) {
		vector3 cross_product{ this->y*vec.z - this->z*vec.y, this->z*vec.x - this->x*vec.z, this->x*vec.y - this->y*vec.x };
		return cross_product;
	}

	void set(const vector3 &vec) {
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	void set(const float &x, const float &y, const float &z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}

	void normalize() {
		float norm = sqrt(this->dot(*this));
		this->x = this->x / norm;
		this->y = this->y / norm;
		this->z = this->z / norm;
	}

}Vector3;

typedef struct tri_model {
	int number_triangles, material_count;
	vector_3d *ambient_color, *diffuse_color, *specular_color;
	float *shine;
	face *faces;
}Tri_model;

void init_settings(Tri_model* object);

Tri_model* reading_input_file(const char *FileName, vector_3d *max_pos, vector_3d *min_pos);
