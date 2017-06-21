#include <string.h>

#include <glut.h>
#include "Rasterizer.h"

template <class base_type>
Buffer<base_type>::Buffer(size_t width, size_t height) {
	this->height = height;
	this->width = width;
	this->buffer.reserve(height);
	//for (size_t i = 0; i < height; i++)
	//{
	std::vector<base_type> column(width);
	this->buffer.assign(height, column);
	//}
}

template <class base_type>
void Buffer<base_type>::resize(size_t width, size_t height) {
	this->height = height;
	this->width = width;
	this->buffer.resize(height);
	//for (size_t i = 0; i < height; i++)
	//{
	std::vector<base_type> column;
	column.resize(width);
	this->buffer.assign(height, column);
	//}
}

template <class base_type>
size_t Buffer<base_type>::get_width() {
	return width;
}

template <class base_type>
size_t Buffer<base_type>::get_height() {
	return height;
}

template <class base_type>
void Buffer<base_type>::set_pos(size_t i, size_t j, const base_type& data) {
	this->buffer[i][i] = data;
}


Rasterizer::Rasterizer(size_t width, size_t height) {
	this->height = height;
	this->width = width;
	this->z_buffer = new Buffer<float>(width, height);
	this->color_buffer = new Buffer<color>(width, height);
}

Rasterizer::~Rasterizer() {
	delete this->color_buffer;
	delete this->z_buffer;
}

void Rasterizer::resize(size_t width, size_t height) {
	this->color_buffer->resize(width, height);
	this->z_buffer->resize(width, height);

}

size_t Rasterizer::get_height() {
	return this->height;
}

size_t Rasterizer::get_width() {
	return this->width;
}

void Rasterizer::include_triangle(Triangle triangle) {
	this->triangles.push_back(triangle);
}

void Rasterizer::rasterize_triangles() {
	for (auto triangle = triangles.begin(); triangle < triangles.end(); triangle++) {
		triangle->rasterize_triangle(true);
	}
}