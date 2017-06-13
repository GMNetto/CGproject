#include <vector>
#include "Triangle.h"
#include <algorithm>
#include "Rasterizer.h"

bool compare_vector_3d(const vertex& v1, const vertex& v2) {
	return v1.coordinates.y < v2.coordinates.y;
}

Triangle::Triangle(vertex p1, vertex p2, vertex p3, char **z_buffer, char *color_buffer, size_t width, size_t height) {
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

void Triangle::interpolate_begin_line(int start, int complete_line, int remaining, bool first, float other_interval, float other_delta_y) {
	float start_y = this->points[start].coordinates.y*height;
	float start_x = this->points[start].coordinates.x*width;
	float complete_line_y = this->points[complete_line].coordinates.y*height;
	float complete_line_x = this->points[complete_line].coordinates.x*width;
	float remaining_y = this->points[remaining].coordinates.y*height;
	float remaining_x = this->points[remaining].coordinates.x*width;

	if (mode != 2)
		interpolate_row(height - start_y, start_x, this->points[start].color, this->points[start].coordinates.z,
			complete_line_x, this->points[complete_line].color, this->points[complete_line].coordinates.z);
	int limit_y = this->points[remaining].coordinates.y * this->height;

	int new_delta_x = ceil(remaining_x - complete_line_x);
	int new_delta_y = ceil(remaining_y - complete_line_y);
	float new_interval = 0;
	if (new_delta_y != 0)
		new_interval = float(new_delta_x) / new_delta_y;

	if (first) {
		//Quando passar do meio triangulo para o outro, existe o pulo, então precisa-se compensar o valor do X
		// Com novo_x = segundo_vertice.x + incremento.x*(ceil(y) - y)
		interpolate_finish(new_interval, other_interval, new_delta_y, other_delta_y, limit_y, start_y
			, complete_line_x, start_x,
			points[complete_line], points[start], points[remaining]);
	}
	else {
		interpolate_finish(other_interval, new_interval, other_delta_y, new_delta_y, limit_y, start_y
			, complete_line_x, start_x,
			points[complete_line], points[start], points[remaining]);
	}
}

void Triangle::interpolate_begin_normal(int first_end, int last_end, int start, bool first, std::pair<int, int> first_delta,
	std::pair<int, int> second_delta, float first_interval, float second_interval,int scanline) {
	int first_one_y = this->points[first_end].coordinates.y * this->height;
	int first_one_x = this->points[first_end].coordinates.x * this->width;
	int last_one_y = this->points[last_end].coordinates.y * this->height;
	int last_one_x = this->points[last_end].coordinates.x * this->width;

	std::pair<int, int> last_x_s = interpolate_start_0(first_interval, second_interval, first_delta.second,
		second_delta.second, first_one_y, scanline);
	
	std::pair<int, int> new_delta = calculate_delta(last_end, first_end);
	float new_interval = 0;
	if (new_delta.second != 0)
		new_interval = float(new_delta.first) / new_delta.second;

	if (first_one_y == last_one_y) {
		interpolate_row(height - first_one_y, first_one_x, this->points[first_end].color, this->points[first_end].coordinates.z,
			last_one_x, this->points[last_end].color, this->points[last_end].coordinates.z);
	}
	else {
		if (first) {
			interpolate_finish(new_interval, second_interval, first_one_y, last_one_y, last_one_y, first_one_y, last_x_s.first, last_x_s.second,
				points[first_end], points[start], points[last_end]);
		}
		else {
			interpolate_finish(first_interval, new_interval, first_one_y, last_one_y, last_one_y, first_one_y, last_x_s.first, last_x_s.second,
				points[first_end], points[start], points[last_end]);
		}
	}
}

void Triangle::draw_pixel(int x, int y, float z, vector_3d color) {
	if (z < z_buffer[y][x] && x < height - 1) {
		this->z_buffer[y][x] = z;
		this->color_buffer[3 * x + 3 * 500 * y + 0] = color.x*255;
		this->color_buffer[3 * x + 3 * 500 * y + 1] = color.y*255;
		this->color_buffer[3 * x + 3 * 500 * y + 2] = color.z*255;
	}
}

void Triangle::draw_line(int x1, int x2, int y, float z1, float z2, vector_3d c1, vector_3d c2, bool wireframe) {
	if (wireframe) {
		return;
	}
	int start_x = std::min(x1, x2), end_x = std::max(x1, x2);
	bool order = false;
	if (x1 - x2) {
		order = true;
	}
	for (int i = start_x; i < end_x; i++)
	{
		if (i < 0) {
			int a = 2;
		}
		float div = (float(x2) - x1);
		float factor = float(i - x1) / div;
		float z = z1*factor + z2*(1 - factor);
		vector_3d c = c1*factor + c2*(1 - factor);
		if (order) {
			z = z1*(1 - factor) + z2*(factor);
			c = c1*(1 - factor) + c2*(factor);
		}
		draw_pixel(i, y, z, c);
	}
}

//O x pode ocorrer distante do pixel vizinho, assim  o x discretizado -> x_d = x_0 + incremento*(y - ceil(y))
void Triangle::rasterize_triangle() {
	vector_3d v0 = points[0].coordinates, v1 = points[1].coordinates, v2 = points[2].coordinates;
	vector_3d c0 = points[0].color, c1 = points[1].color, c2 = points[2].color;
	float z0 = points[0].coordinates.z, z1 = points[1].coordinates.z, z2 = points[2].coordinates.z;
	float eps = 0.5;
	if (abs(v1.y - v0.y) < eps) {
		if (abs(v1.y - v2.y) > 1) {
			float inc_x1 = (v2.x - v0.x) / (v2.y - v0.y),
				inc_z1 = (v2.z - v0.z) / (v2.y - v0.y);
			vector_3d inc_c1 = (c2 - c0) / (v2.y - v0.y);

			float inc_x2 = (v2.x - v1.x) / (v2.y - v1.y),
				inc_z2 = (v2.z - v1.z) / (v2.y - v1.y);
			vector_3d inc_c2 = (c2 - c1) / (v2.y - v1.y);

			int y = ceil(v0.y);
			float x1 = v0.x + inc_x1*(y - v0.y);
			float x2 = v1.x + inc_x2*(y - v1.y);
			float z_1 = z0 + inc_z1*(y - v0.y);
			float z_2 = z1 + inc_z2*(y - v1.y);
			vector_3d color1 = c0 + inc_c1*(y - v0.y);
			vector_3d color2 = c1 + inc_c2*(y - v1.y);
			draw_line(int(x1), int(x2), y, z_1, z_2, color1, color2, false);

			for (; y < ceil(v2.y); y++)
			{
				draw_line(x1, x2, y, z1, z2, c1, c2, false);
				x1 += inc_x1;
				x2 += inc_x2;
				z1 += inc_z1;
				z2 += inc_z2;
				c1 = c1 + inc_c1;
				c2 = c2 + inc_c2;
			}
		}
	}
	else {
		float inc_x1 = (v1.x - v0.x) / (v1.y - v0.y);
		float inc_z1 = (z1 - z0) / (v1.y - v0.y);
		vector_3d inc_c1 = (c1 - c0) / (v1.y - v0.y);

		float inc_x2 = (v2.x - v0.x) / (v2.y - v0.y);
		float inc_z2 = (z2 - z0) / (v2.y - v0.y);
		vector_3d inc_c2 = (c2 - c0) / (v2.y - v0.y);

		int y = ceil(v0.y);
		float x1 = v0.x + inc_x1*(y - v0.y);
		float x2 = v0.x + inc_x2*(y - v0.y);
		float z1 = z0 + inc_z1*(y - v0.y);
		float z2 = z0 + inc_z2*(y - v0.y);
		vector_3d c1 = c0 + inc_c1*(y - v0.y);
		vector_3d c2 = c0 + inc_c2*(y - v0.y);
		bool changed = false;
		for (;y < ceil(v2.y); y++)
		{
			if (!changed && y > ceil(v1.y)) {
				inc_x1 = (v2.x - v1.x) / (v2.y - v1.y);
				float inc_z1 = (z2 - z1) / (v2.y - v1.y);
				vector_3d inc_c1 = (c2 - c1) / (v2.y - v1.y);

				x1 = v1.x + inc_x1*(y - v1.y);
				z1 = z1 + inc_z1*(y - v1.y);
				vector_3d c1 = c1 + inc_c1*(y - v1.y);

				changed = true;
			}
			draw_line(x1, x2, y, z1, z2, c1, c2, false);
			x1 += inc_x1;
			x2 += inc_x2;
			z1 += inc_z1;
			z2 += inc_z2;
			c1 = c1 + inc_c1;
			c2 = c2 + inc_c2;
		}
		if (abs(v2.y - v1.y) < eps) {
			draw_line(x1, x2, y, z1, z2, c1, c2, false);
		}
	}

	/*std::pair<int, int> first_delta = calculate_delta(1, 0);
	float first_interval = 0;
	//Usar um eps para a comparação com abs
	if (first_delta.second != 0)
		first_interval = float(first_delta.first) / first_delta.second;
	
	std::pair<int, int> second_delta = calculate_delta(2, 0);

	//Não faz sentido porques está ordenado
	float second_interval = 0;
	if (second_delta.second != 0)
		second_interval = float(second_delta.first) / second_delta.second;

	int limit_y = std::min(first_delta.second, second_delta.second) + this->points[0].coordinates.y*height;

	int scanline = int(this->get_Y() * this->height);

	if (abs(first_delta.second) <= 1)//passar para epsilon
		interpolate_begin_line(0, 1, 2, true, second_interval, second_delta.second);
	else if (abs(second_delta.second) <= 1) {
		interpolate_begin_line(0, 2, 1, false, first_interval, first_delta.second);
	} else if (this->points[1].coordinates.y < this->points[2].coordinates.y) {
		interpolate_begin_normal(1, 2, 0, true, first_delta, second_delta, first_interval, second_interval, scanline);
	} else {
		interpolate_begin_normal(2, 1, 0, false, first_delta, second_delta, first_interval, second_interval, scanline);
	}*/
}

void Triangle::interpolate_finish(float first_delta_x, float second_delta_x,
	float first_delta_y, float second_delta_y, int limit_y, int scanline, int first_x,
	int second_x, vertex& first, vertex& second, vertex& last) {
	float last_first_x = first_x, new_first_x = first_x, last_second_x = second_x, new_second_x = second_x;

	vector_3d& color_first = first.color, color_second = second.color, color_last = last.color;

	float counter = scanline - points[0].coordinates.y;

	while (scanline < limit_y) {
		new_first_x = last_first_x + first_delta_x;
		new_second_x = last_second_x + second_delta_x;

		vector_3d color_beginning = color_first*(1 - counter/first_delta_y) + color_last*(counter/first_delta_y),
			color_ending = color_second*(1 - counter/second_delta_y) + color_last*(counter/second_delta_y);

		float first_z_interpolated = first.coordinates.z * (1 - counter/first_delta_y) + second.coordinates.z*(counter/first_delta_y),
			second_z_interpolated = second.coordinates.z * (1 - counter/second_delta_y) + second.coordinates.z * (counter/second_delta_y);

		write(floor(new_first_x), height - scanline, first_z_interpolated, color_beginning);
		write(floor(new_second_x), height - scanline, second_z_interpolated, color_ending);

		if(mode == 0)
			interpolate_row(height - scanline, floor(new_first_x), color_beginning, first_z_interpolated,
				floor(new_second_x), color_ending, second_z_interpolated);

		scanline += 1;
		counter += 1;

		last_first_x = new_first_x;
		last_second_x = new_second_x;
		if (mode == 2)
			break;
	}

}


std::pair<int, int> Triangle::interpolate_start_0(float first_delta_x, float second_delta_x,
	float first_delta_y, float second_delta_y, int limit_y, int scanline) {
	
	float first_last_x = points[0].coordinates.x*this->width, second_last_x = first_last_x;
	vector_3d& color_p0 = points[0].color, color_p1 = points[1].color, color_p2 = points[2].color;

	float counter = 0;

	while (scanline < limit_y) {
		first_last_x += first_delta_x;

		vector_3d color_beginning = color_p0*(1 - counter/first_delta_y) + color_p1*(counter/first_delta_y),
			color_ending = color_p0*(1 - counter/second_delta_y) + color_p2*(counter/second_delta_y);

		float first_z_interpolated = points[0].coordinates.z*(1 - counter/first_delta_y) + points[1].coordinates.z*(counter/first_delta_y),
			second_z_interpolated = points[0].coordinates.z*(1 - counter/second_delta_y) + points[2].coordinates.z*(counter/second_delta_y);

		write(floor(first_last_x), height - scanline, first_z_interpolated, color_beginning);
		second_last_x += second_delta_x;
		write(floor(second_last_x), height - scanline, second_z_interpolated,color_ending);

		if(mode == 0)
			interpolate_row(height - scanline, floor(first_last_x), color_beginning, first_z_interpolated, floor(second_last_x), color_ending, second_z_interpolated);

		scanline += 1;
		counter += 1;

		if (mode == 2)
			break;
	}
	return std::make_pair(floor(first_last_x), floor(second_last_x));
}

void Triangle::interpolate_row(int scanline, int start, vector_3d& color_start, float first_z, int end, vector_3d& color_end, float second_z) {
	float factor = 0;
	if (start != end)
		factor = 1 / (end - start);
	//p = (x - x0)/(x0 - x1), em que x_i são os extremos, e o outro é 1 - p.
	if (start < end) {
		for (size_t i = start; i < end; i++)
		{
			write(i, scanline, first_z*(1 - factor) + second_z*factor, color_start*(1 - factor) + color_end*factor);
		}
	}
	else {
		for (size_t i = end; i < start; i++)
		{
			write(i, scanline, second_z*(1 - factor) + first_z*factor, color_end*(1 - factor) + color_start*factor);
		}
	}
	
}

void Triangle::set_buffers(size_t width, size_t height, char **z_buffer, char *color_buffer) {
	this->z_buffer = z_buffer;
	this->color_buffer = color_buffer;
	this->width = width;
	this->height = height;
}