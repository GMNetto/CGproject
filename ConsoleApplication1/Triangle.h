#pragma once
#include <Util.h>
#include <vector>

struct vertex
{
	vector_3d coordinates, color, normal, world_position, texture;
	float w;
	vertex(vector_3d coordinates, vector_3d color, vector_3d normal, vector_3d world, float w, vector_3d texture){
		this->normal = normal;
		this->coordinates = coordinates;
		this->color = color;
		this->world_position = world;
		this->w = w;
		this->texture = texture;
	}
};

class Triangle
{
public:
	Triangle(vertex p1, vertex p2, vertex p3, float **z_buffer, float *color_buffer, size_t width, size_t height, vector_3d face_normal);
	vertex get_vertex(int index);
	float get_Y();

	void set_buffers(size_t width, size_t height, float **z_buffer, float *color_buffer);

	void rasterize_triangle(bool perspective_correct);

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
			vector_3d temp;
			if (this->lightiting_type == 0) {
				temp = this->get_lighting_color(points[i].normal, points[i].world_position);
			}
			else {
				temp = this->get_lighting_color(this->face_normal, points[i].world_position);
			}
			points[i].color.x = temp.x;
			points[i].color.y = temp.y;
			points[i].color.z = temp.z;
		}
	}

	void set_texture(unsigned char* texture, int texture_mode, int t_width, int t_height) {
		this->texture = texture;
		this->texture_height = t_height;
		this->texture_width = t_width;
		this->texture_mode = texture_mode;
		this->create_mipmaps();
	}

	void set_lighting_type(int lighting_type) {
		this->lightiting_type = lighting_type;
	}

	void set_attenuation(float attenuation) {
		this->attenuation = attenuation;
	}

	void set_texture_type(int texture_type) {
		this->texture_type = texture_type;
	}

private:
	unsigned char *texture;
	int texture_width, texture_height, number_of_levels;
	float **mipmaps;

	int texture_mode = 0;
	float **z_buffer;
	float *color_buffer;
	std::vector<vertex> points;
	size_t width, height;
	int lightiting_type = 0;
	vector_3d face_normal;
	float attenuation = 1;
	int texture_type = 0;

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
		//illumination_direction.normalize();
		vector_3d light_direction = illumination_direction - world_position,
			camera_direction = camera_position - world_position;
		light_direction.normalize();
		camera_direction.normalize();
		vector_3d R = normal * 2 * light_direction.dot(normal) - light_direction;
		R.normalize();
		float r_color = ambient_color.x +attenuation*illumination_color.x*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		float g_color = ambient_color.y +attenuation*illumination_color.y*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		float b_color = ambient_color.z +attenuation*illumination_color.z*(0.5*normal.dot(light_direction) + 0.5*R.dot(camera_direction));
		return vector_3d(r_color, g_color, b_color);
	}

	std::pair<int, int> calculate_delta(int first_point, int second_point);

	void interpolate_begin_line(int start, int complete_line, int remaining, bool first, float other_interval, float other_delta_y);

	void interpolate_begin_normal(int first_end, int last_end, int start, bool first, std::pair<int, int> first_delta,
		std::pair<int, int> second_delta, float first_interval, float second_interval, int scanline);

	void draw_pixel(int x, int y, float z, vector_3d color);

	void draw_line(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, vector_3d t1, vector_3d t2, bool wireframe, bool perspective_correct, float level);

	void set_textures_in_vertices(vector_3d* v0, vector_3d* v1, vector_3d* v2);

	void draw_texture(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe,
		float t_begin, float t_end, float t_y);

	void draw_texture_2(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe,
		float t_begin, float t_end, float t_y, float t_w_begin, float t_w_end);

	vector_3d get_texture_sample(int x, int y);

	vector_3d sample_texture(vector_3d texture_coord, float level);

	void create_mipmaps();

	float *downsample(float *previous_level, int current_width, int current_height);

	vector_3d get_texture_mipmap(int x, int y, int level);

};
