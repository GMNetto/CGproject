#pragma once
#include <Util.h>
#include <vector>

struct vertex
{
	vector_3d coordinates, color, normal, world_position;
	vertex(vector_3d coordinates, vector_3d color, vector_3d normal, vector_3d world){
		this->normal = normal;
		this->coordinates = coordinates;
		this->color = color;
		this->world_position = world;
	}
};

class Triangle
{
public:
	Triangle(vertex p1, vertex p2, vertex p3, char **z_buffer, char *color_buffer, size_t width, size_t height);
	vertex get_vertex(int index);
	float get_Y();

	void set_buffers(size_t width, size_t height, char **z_buffer, char *color_buffer);

	void rasterize_triangle();

	void set_lighting(bool state) {
		this->mode_lighting = state;
	}

	void set_mode(int mode) {
		this->mode = mode;
	}

	void set_illumination_settings(const vector_3d& a_l, const vector_3d& i_c, const vector_3d& i_d, const vector_3d& camera_p) {
		this->ambient_color = a_l;
		this->illumination_color = i_c;
		this->illumination_direction = i_d;
		this->camera_position = camera_p;
	}

	void set_lighting_colors() {
		for (size_t i = 0; i < 3; i++)
		{
			vector_3d temp = this->get_lighting_color(points[i].normal, points[i].world_position);
			points[i].color.x = temp.x;
			points[i].color.y = temp.y;
			points[i].color.z = temp.z;
		}
	}

private:
	char **z_buffer;
	char *color_buffer;
	std::vector<vertex> points;
	size_t width, height;

	bool mode_lighting;

	int mode;

	vector_3d ambient_color, illumination_color, illumination_direction, camera_position;

	float specularity, diffuse_k, spec_k;

	void write(size_t x, size_t y, float z, const vector_3d& color);

	std::pair<int, int> interpolate_start_0(float first_delta_x, float second_delta_x,
		float first_delta_y, float second_delta_y, int limit_y, int scanline);
	void interpolate_row(int scanline, int start, vector_3d& color_start, float first_z, int end, vector_3d& color_end, float second_z);

	void interpolate_finish(float first_delta_x, float second_delta_x,
		float first_delta_y, float second_delta_y, int limit_y, int scanline, int first_x,
		int second_x, vertex& first, vertex& second, vertex& last);

	vector_3d get_lighting_color(vector_3d& normal, vector_3d& world_position) {
		illumination_direction.normalize();
		vector_3d light_direction = illumination_direction - world_position,
			camera_direction = camera_position - world_position;
		light_direction.normalize();
		camera_direction.normalize();
		vector_3d R = normal * 2 * light_direction.dot(normal) - light_direction;
		R.normalize();
		float r_color = ambient_color.x +0.5*illumination_color.x*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		float g_color = ambient_color.y +0.5*illumination_color.y*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		float b_color = ambient_color.z +0.5*illumination_color.z*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		return vector_3d(r_color, g_color, b_color);
	}

	std::pair<int, int> calculate_delta(int first_point, int second_point);

	void interpolate_begin_line(int start, int complete_line, int remaining, bool first, float other_interval, float other_delta_y);

	void interpolate_begin_normal(int first_end, int last_end, int start, bool first, std::pair<int, int> first_delta,
		std::pair<int, int> second_delta, float first_interval, float second_interval, int scanline);

	void draw_pixel(int x, int y, float z, vector_3d color);

	void draw_line(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe);

};
