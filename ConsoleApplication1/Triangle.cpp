#include <vector>
#include <iostream>
#include "Triangle.h"
#include <algorithm>
#include "Rasterizer.h"

bool compare_vector_3d(const vertex& v1, const vertex& v2) {
	return v1.coordinates.y < v2.coordinates.y;
}

Triangle::Triangle(vertex p1, vertex p2, vertex p3, float **z_buffer, float *color_buffer, size_t width, size_t height, vector_3d face_normal) {
	this->points.reserve(3);
	this->points.push_back(p1);
	this->points.push_back(p2);
	this->points.push_back(p3);
	std::sort(this->points.begin(), this->points.end(), compare_vector_3d);
	this->z_buffer = z_buffer;
	this->color_buffer = color_buffer;
	this->width = width;
	this->height = height;
	this->mode_lighting = false;
	this->mode = 0;
	this->face_normal = face_normal;
}

vertex Triangle::get_vertex(int index) {
	return this->points[index];
}

float Triangle::get_Y() {
	return this->points[0].coordinates.y;
}

void Triangle::write(size_t x, size_t y, float z, const vector_3d& color) {
	//float val = z_buffer[x][y];
	if (z < z_buffer[x][y]) {
		this->z_buffer[x][y] = z;
		this->color_buffer[3 * x + 3 * 500 * y + 0] = color.x;
		this->color_buffer[3 * x + 3 * 500 * y + 1] = color.y;
		this->color_buffer[3 * x + 3 * 500 * y + 2] = color.z;
	}
}

std::pair<int, int> Triangle::calculate_delta(int first_point, int second_point) {
	///Delta deveria ser float
	int delta_x = ceil(this->points[first_point].coordinates.x*width - this->points[second_point].coordinates.x*width);
	int delta_y = ceil(this->points[first_point].coordinates.y*height - this->points[second_point].coordinates.y*height);
	return std::make_pair(delta_x, delta_y);
}


vector_3d Triangle::get_texture_sample(int x, int y) {
	unsigned char red = texture[3 * y * 64 + 3 * x + 0];
	unsigned char green = texture[3 * y * 64 + 3 * x + 1];
	unsigned char blue = texture[3 * y * 64 + 3 * x + 2];
	return vector_3d(float(red)/255, float(green)/255, float(blue)/255);
}

void Triangle::draw_pixel(int x, int y, float z, vector_3d color) {
	if (z <= z_buffer[y][x] && x < height - 1) {
		this->z_buffer[y][x] = z;
		this->color_buffer[3 * x + 3 * 500 * y + 0] = color.x;
		this->color_buffer[3 * x + 3 * 500 * y + 1] = color.y;
		this->color_buffer[3 * x + 3 * 500 * y + 2] = color.z;
	}
}

void Triangle::draw_texture(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe,
	float t_begin, float t_end, float t_y) {
	if (wireframe) {
		draw_pixel(x1, y, z1, c1);
		draw_pixel(x2, y, z2, c2);
	}
	else {
		if (x1 < x2) {
			float inc_texture_x = (t_end - t_begin) / (x2 - x1); // texture
			for (int x = x1; x <= x2; x++) {
				float p = (x - x1) / (float)(x2 - x1);
				float z = z1 * (1 - p) + z2 * p;
				vector_3d c = c1 * (1 - p) + c2 * p;
				if (texture_mode) { //texture
					int i = int(t_y);
					int j = int(t_begin);
					unsigned char red = texture[3 * i * 64 + 3 * j + 0];
					unsigned char green = texture[3 * i * 64 + 3 * j + 1];
					unsigned char blue = texture[3 * i * 64 + 3 * j + 2];
					draw_pixel(x, y, z, vector_3d(red/255.0, green/255.0, blue/255.0));
				}
				else {
					draw_pixel(x, y, z, c);
				}
				t_begin += inc_texture_x; // texture
			}
		}
		else {
			float inc_texture_x = (t_end - t_begin) / (x1 - x2); // texture
			for (int x = x2; x <= x1; x++) {
				float p = (x - x2) / (float)(x1 - x2);
				float z = z2 * (1 - p) + z1 * p;
				vector_3d c = c2 * (1 - p) + c1 * p;
				if (texture_mode) { //texture
					int i = int(t_y);
					int j = int(t_begin);
					unsigned char red = texture[3 * i * 64 + 3 * j + 0];
					unsigned char green = texture[3 * i * 64 + 3 * j + 1];
					unsigned char blue = texture[3 * i * 64 + 3 * j + 2];
					draw_pixel(x, y, z, vector_3d(red/255.0, green/255.0, blue/255.0));
				}
				else {
					draw_pixel(x, y, z, c);
				}
				t_begin += inc_texture_x; // texture
			}
		}
	}
}

void Triangle::draw_texture_2(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe,
	float t_begin, float t_end, float t_y, float t_w_begin, float t_w_end) {
	if (wireframe) {
		draw_pixel(x1, y, z1, c1);
		draw_pixel(x2, y, z2, c2);
	}
	else {
		if (x1 < x2) {
			float inc_texture_x = (t_end - t_begin) / (x2 - x1); // texture
			float inc_texture_w = (t_w_end - t_w_begin) / (x2 - x1);// correct
			for (int x = x1; x <= x2; x++) {
				float p = (x - x1) / (float)(x2 - x1);
				float z = z1 * (1 - p) + z2 * p;
				vector_3d c = c1 * (1 - p) + c2 * p;
				if (texture_mode) { //texture
					int i = int(t_y/t_w_begin); //correct
					int j = int(t_begin/t_w_begin); //correct
					//std::cout << i << " " << j << std::endl;
					unsigned char red = texture[3 * i * 64 + 3 * j + 0];
					unsigned char green = texture[3 * i * 64 + 3 * j + 1];
					unsigned char blue = texture[3 * i * 64 + 3 * j + 2];
					draw_pixel(x, y, z, vector_3d(red / 255.0, green / 255.0, blue / 255.0));
				}
				else {
					draw_pixel(x, y, z, c);
				}
				t_begin += inc_texture_x; // texture
				t_w_begin += inc_texture_w; // correct
			}
		}
		else {
			float inc_texture_x = (t_end - t_begin) / (x1 - x2); // texture
			float inc_texture_w = (t_w_end - t_w_begin) / (x1 - x2);// correct
			for (int x = x2; x <= x1; x++) {
				float p = (x - x2) / (float)(x1 - x2);
				float z = z2 * (1 - p) + z1 * p;
				vector_3d c = c2 * (1 - p) + c1 * p;
				if (texture_mode) { //texture
					int i = int(t_y/t_w_begin);
					int j = int(t_begin/t_w_begin);
					unsigned char red = texture[3 * i * 64 + 3 * j + 0];
					unsigned char green = texture[3 * i * 64 + 3 * j + 1];
					unsigned char blue = texture[3 * i * 64 + 3 * j + 2];
					draw_pixel(x, y, z, vector_3d(red / 255.0, green / 255.0, blue / 255.0));
				}
				else {
					draw_pixel(x, y, z, c);
				}
				t_begin += inc_texture_x; // texture
				t_w_begin += inc_texture_w; // correct
			}
		}
	}
}

float* Triangle::downsample(float *previous_level, int current_width, int current_height) {
	int next_width = current_width >> 1, next_height = current_height >> 1;
	float *new_level = new float[next_width * next_height * 3];
	for (int i = 0; i < next_height; i++)
	{
		int previous_y = i << 1;
		for (int j = 0; j < next_width; j++)
		{
			int previous_x = j << 1;
			for (int k = 0; k < 3; k++)
			{
				new_level[i * next_width * 3 + j * 3 + k] = (previous_level[3 * previous_y * current_width + previous_x * 3 + k] +
					previous_level[3 * previous_y * current_width + (previous_x + 1) * 3 + k] +
					previous_level[3 * (previous_y + 1) * current_width + previous_x * 3 + k] +
					previous_level[3 * (previous_y + 1) * current_width + (previous_x + 1) * 3 + k]) / 4;
			}
		}
	}
	return new_level;
}

void Triangle::create_mipmaps() {
	number_of_levels = ceil(log2(texture_height));
	mipmaps = new float*[number_of_levels];
	mipmaps[0] = new float[3 * texture_height * texture_width];
	for (size_t i = 0; i < texture_height; i++)
	{
		for (size_t j = 0; j < texture_width; j++)
		{
			for (size_t k = 0; k < 3; k++)
			{
				mipmaps[0][3 * i * 64 + 3 * j + k] = texture[3 * i * 64 + 3 * j + k];
			}
		}
	}
	int current_width = texture_width, current_height = texture_height;
	for (int i = 1; i < number_of_levels; i++)
	{
		mipmaps[i] = downsample(mipmaps[i - 1], current_width, current_height);
		current_width >>= 1;
		current_height >>= 1;
	}
}

vector_3d Triangle::get_texture_mipmap(int x, int y, int level) {
	int width_level = texture_width << level;
	int height_level = texture_height << level;
	int x_level = int(std::min(x, width_level));
	int y_level = int(std::min(y, height_level));
	unsigned char red = mipmaps[level][y_level * width_level * 3 + 3 * x_level + 0];
	unsigned char green = mipmaps[level][y_level * width_level * 3 + 3 * x_level + 1];
	unsigned char blue = mipmaps[level][y_level * width_level * 3 + 3 * x_level + 2];
	return vector_3d(red, green, blue);
}

vector_3d Triangle::sample_texture(vector_3d texture_coord, float level) {
	texture_coord.x *= 64;
	texture_coord.y *= 64;

	if (this->texture_type == 0) {
		vector_3d s0 = this->get_texture_sample(floor(texture_coord.x), floor(texture_coord.y));
		vector_3d s1 = this->get_texture_sample(ceil(texture_coord.x), floor(texture_coord.y));
		vector_3d s2 = this->get_texture_sample(floor(texture_coord.x), ceil(texture_coord.y));
		vector_3d s3 = this->get_texture_sample(ceil(texture_coord.x), ceil(texture_coord.y));
		return (s0 + s1 + s2 + s3) / 4;
	}
	else if (this->texture_type == 1) {
		return this->get_texture_sample(std::round(texture_coord.x), std::round(texture_coord.y));
	}
	else {
		int first_level = level;
		int second_level = level + 1;
		float first_x = texture_coord.x / (1 << first_level);
		float first_y = texture_coord.y / (1 << first_level);
		float second_x = texture_coord.x / (1 << second_level);
		float second_y = texture_coord.y / (1 << second_level);
		vector_3d color_first_level = get_texture_mipmap(first_x, first_y, level);
		vector_3d color_second_level = get_texture_mipmap(second_x, second_y, level);
		return color_first_level * (1 - (level - first_level)) + color_second_level *(1 - (1 - (level - first_level)));
	}
}

void Triangle::draw_line(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, vector_3d t1, vector_3d t2, bool wireframe, bool perspective_correct, float level) {
	if (wireframe) {
		draw_pixel(x1, y, z1, c1);
		draw_pixel(x2, y, z2, c2);
	}
	else {
		if (x1 < x2) {
			for (int x = x1; x < x2; x++) {
				float p = (x - x1) / (float)(x2 - x1);
				float z = z1 * (1 - p) + z2 * p;
				float w = t1.z * (1 - p) + t2.z * p;
				vector_3d c;
				if (this->texture_mode) {
					vector_3d t = t1 * (1 - p) + t2 * p;

					if(perspective_correct)
						c = sample_texture(t / w, level);
					else
						c = sample_texture(t, level);
				}
				else {
					if(perspective_correct)
						c = (c1 * (1 - p) + c2 * p) / w;
					else
						c = (c1 * (1 - p) + c2 * p);
				}
				draw_pixel(x, y, z, c);
			}
		}
		else {
			for (int x = x2; x < x1; x++) {
				float p = (x - x2) / (float)(x1 - x2);
				float z = z2 * (1 - p) + z1 * p;
				float w = t2.z * (1 - p) + t1.z *p;
				vector_3d c;
				if (this->texture_mode) {
					vector_3d t = t2 * (1 - p) + t1 * p;

					if (perspective_correct)
						c = sample_texture(t / w, level);
					else
						c = sample_texture(t, level);
				}
				else {
					if (perspective_correct)
						c = (c2 * (1 - p) + c1 * p) / w;
					else
						c = (c2 * (1 - p) + c1 * p);
				}
				draw_pixel(x, y, z, c);
			}
		}
	}
}

void Triangle::set_textures_in_vertices(vector_3d* t0, vector_3d* t1, vector_3d* t2) {
	vector_3d v0 = points[0].coordinates, v1 = points[1].coordinates, v2 = points[2].coordinates;
	t0->y = 0;
	t1->y = 63 * ((v1.y - v0.y) / (v2.y - v0.y));
	t1->y /= points[1].w; //correct
	t2->y = 63;
	t2->y /= points[2].w; //correct
	t0->z /= points[0].w; //correct
	t1->z /= points[1].w; //correct
	t2->z /= points[2].w; //correct
	if (v0.x < v1.x && v0.x < v2.x) {
		t0->x = 0;
		if (v1.x < v2.x) {
			t1->x = 63 * ((v1.x - v0.x) / (v2.x - v0.x));
			t2->x = 63;
		}
		else {
			t1->x = 63;
			t2->x = 63 * ((v2.x - v0.x) / (v1.x - v0.x));
		}
	}
	else if (v1.x < v0.x && v1.x < v2.x) {
		t1->x = 0;
		if (v0.x <v2.x) {
			t2->x = 63;
			t0->x = 63 * ((v0.x - v1.x) / (v2.x - v1.x));
		}
		else {
			t0->x = 63;
			t2->x = 63 * ((v2.x - v1.x) / (v0.x - v1.x)) ;
		}
	}
	else {
		t2->x = 0;
		if (v0.x < v1.x) {
			t1->x = 63;
			t0->x = 63 * ((v0.x - v2.x) / (v1.x - v2.x));
		}
		else {
			t0->x = 63;
			t1->x = 63 * ((v1.x - v2.x) / (v0.x - v2.x));
		}
	}
	t0->x /= this->points[0].w; //correct
	t1->x /= this->points[1].w; //correct
	t2->x /= this->points[2].w; //correct
}

//O x pode ocorrer distante do pixel vizinho, assim  o x discretizado -> x_d = x_0 + incremento*(y - ceil(y))
void Triangle::rasterize_triangle(bool perspective_correct) {
	vector_3d v0 = points[0].coordinates, v1 = points[1].coordinates, v2 = points[2].coordinates;
	vector_3d color0 = points[0].color, color1 = points[1].color, color2 = points[2].color;
	vector_3d t0 = points[0].texture, t1 = points[1].texture, t2 = points[2].texture;
	t0.z = 1;
	t1.z = 1;
	t2.z = 1;
	//I have to use the coordinates given by the file
	//this->set_textures_in_vertices(&t0, &t1, &t2);

	if (perspective_correct) {
		color0 = color0 / points[0].w;
		color1 = color1 / points[0].w;
		color2 = color2 / points[0].w;
		t0 = t0 / points[0].w;
		t1 = t1 / points[0].w;
		t2 = t2 / points[0].w;
	}
	
	if (mode == 2) {
		this->draw_pixel(int(v0.x), int(v0.y), v0.z, color0);
		this->draw_pixel(int(v1.x), int(v1.y), v1.z, color1);
		this->draw_pixel(int(v2.x), int(v2.y), v2.z, color2);
		return;
	}

	//Define mipmap  level
	float dx = std::max(abs(v0.x - v1.x), std::max(abs(v0.x - v2.x), abs(v1.x - v2.x)));
	float dy = std::max(abs(v0.y - v1.y), std::max(abs(v0.y - v2.y), abs(v1.y - v2.y)));
	float ds = std::max(abs(t0.x - t1.x), std::max(abs(t0.x - t2.x), abs(t1.x - t2.x)));
	float dt = std::max(abs(t0.y - t1.y), std::max(abs(t0.y - t2.y), abs(t1.y - t2.y)));

	float dx_part = sqrt((ds / dx)*(ds / dx) + (dt / dx)*(dt / dx));
	float dy_part = sqrt((ds / dy)*(ds / dy) + (dt / dy)*(dt / dy));

	float level = std::log2(std::max(dx_part, dy_part));

	float eps = 1e-3;
	if (abs(v1.y - v0.y) < eps) {
		float inc_x1 = (v2.x - v0.x) / (v2.y - v0.y),
			inc_z1 = (v2.z - v0.z) / (v2.y - v0.y);
		vector_3d inc_c1 = (color2 - color0) / (v2.y - v0.y);

		vector_3d inc_t1 = (t2 - t0) / (v2.y - v0.y); // texture
		float inc_tw1 = (t2.z - t0.z) / (v2.y - v0.y); // texture
			
		float inc_x2 = (v2.x - v1.x) / (v2.y - v1.y),
			inc_z2 = (v2.z - v1.z) / (v2.y - v1.y);
		vector_3d inc_c2 = (color2 - color1) / (v2.y - v1.y);

		vector_3d inc_t2 = (t2 - t1) / (v2.y - v1.y); // texture
		float inc_tw2 = (t2.z - t1.z) / (v2.y - v1.y); // texture

		int y = ceil(v0.y);
		float x1 = v0.x + inc_x1*(y - v0.y);
		float x2 = v1.x + inc_x2*(y - v1.y);
		float z1 = v0.z + inc_z1*(y - v0.y);
		float z2 = v1.z + inc_z2*(y - v1.y);
		vector_3d c1 = color0 + inc_c1*(y - v0.y);
		vector_3d c2 = color1 + inc_c2*(y - v1.y);
		vector_3d t_first = t0 + inc_t1*(y - v0.y);
		vector_3d t_second = t1 + inc_t2*(y - v0.y);
		
		//Stil have to check this
		draw_line(int(x1), int(x2), y, z1, z2, c1, c2, t_first, t_second, false, perspective_correct, level);

		for (; y < (ceil(v2.y) - 1); y++)
		{
			if(mode == 1)
				draw_line(x1, x2, y, z1, z2, c1, c2, t_first, t_second, true, perspective_correct, level);
			else
				draw_line(x1, x2, y, z1, z2, c1, c2, t_first, t_second, false, perspective_correct, level);
			//draw_texture_2(x1, x2, y, z1, z2, c1, c2, false, t_begin, t_end, t_y, t_begin_w, t_end_w);
			x1 += inc_x1;
			x2 += inc_x2;
			z1 += inc_z1;
			z2 += inc_z2;
			c1 = c1 + inc_c1;
			c2 = c2 + inc_c2;
			t_first = t_first + inc_t1; //texture
			t_second = t_second + inc_t2; //texture
		}
		int a = 2;
	}
	else {
		float inc_x1 = (v1.x - v0.x) / (v1.y - v0.y);
		float inc_z1 = (v1.z - v0.z) / (v1.y - v0.y);
		vector_3d inc_c1 = (color1 - color0) / (v1.y - v0.y);

		vector_3d inc_t1 = (t1 - t0) / (v1.y - v0.y); //texture

		float inc_x2 = (v2.x - v0.x) / (v2.y - v0.y);
		float inc_z2 = (v2.z - v0.z) / (v2.y - v0.y);
		vector_3d inc_c2 = (color2 - color0) / (v2.y - v0.y);

		vector_3d inc_t2 = (t2 - t0) / (v2.y - v0.y); //texture

		int y = (int)ceil(v0.y);
		float x1 = v0.x + inc_x1*(y - v0.y);
		float x2 = v0.x + inc_x2*(y - v0.y);
		float z1 = v0.z + inc_z1*(y - v0.y);
		float z2 = v0.z + inc_z2*(y - v0.y);
		vector_3d c1 = color0 + inc_c1*(y - v0.y);
		vector_3d c2 = color0 + inc_c2*(y - v0.y);
		vector_3d t_first = t0 + inc_t1*(y - v0.y);
		vector_3d t_second = t0 + inc_t2*(y - v0.y);

		bool changed = false;
		for (;y < v2.y; y++)
		{
			if (!changed && y > v1.y && abs(v2.y - v1.y) > eps) {
				inc_x1 = (v2.x - v1.x) / (v2.y - v1.y);
				inc_z1 = (v2.z - v1.z) / (v2.y - v1.y);
				inc_c1 = (color2 - color1) / (v2.y - v1.y);

				inc_t1 = (t2 - t1) / (v2.y - v1.y); //texture

				x1 = v1.x + inc_x1*(y - v1.y);
				z1 = v1.z + inc_z1*(y - v1.y);
				c1 = color1 + inc_c1*(y - v1.y);
				t1 = t1 + inc_t1*(y - v1.y);

				changed = true;
			}
			//if for perspective correctness
			if(mode == 1)
				draw_line(x1, x2, y, z1, z2, c1, c2, t_first, t_second, true, perspective_correct, level);
			else
				draw_line(x1, x2, y, z1, z2, c1, c2, t_first, t_second, false, perspective_correct, level);
			x1 += inc_x1;
			x2 += inc_x2;
			z1 += inc_z1;
			z2 += inc_z2;
			c1 = c1 + inc_c1;
			c2 = c2 + inc_c2;
			t_first = t_first + inc_t1; //texture
			t_second = t_second + inc_t2; //texture

		}
		if (abs(v2.y - v1.y) < eps) {
			draw_line(x1, x2, y, z1, z2, c1, c2, t_first, t_second, false, perspective_correct, level);
		}
	}
}

void Triangle::set_buffers(size_t width, size_t height, float **z_buffer, float *color_buffer) {
	this->z_buffer = z_buffer;
	this->color_buffer = color_buffer;
	this->width = width;
	this->height = height;
}