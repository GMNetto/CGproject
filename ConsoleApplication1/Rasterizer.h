#pragma once
#include "Util.h"
#include "Triangle.h"
#include <vector>
#include <glut.h>

typedef vector_3d color;

template <class base_type>
class Buffer {
public:
	Buffer(size_t width, size_t height);
	void resize(size_t width, size_t height);
	size_t get_width();
	size_t get_height();
	void set_pos(size_t i, size_t j, const base_type& data);
private:
	size_t width, height;
	std::vector< std::vector<base_type > > buffer;
};

class Rasterizer {
public:
	Rasterizer(size_t width, size_t height);
	~Rasterizer();
	void rasterize_triangles();
	void include_triangle(Triangle triangle);

	size_t get_width();
	size_t get_height();
	void resize(size_t width, size_t height);
private:
	size_t width, height;
	Buffer<float> *z_buffer;
	Buffer<color> *color_buffer;
	std::vector<Triangle> triangles;
};
