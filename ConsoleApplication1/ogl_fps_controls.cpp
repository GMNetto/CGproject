#include <glew.h>
#include <glut.h>
#include "matrix4x4f.h"
#include "vector3f.h"
#include <stdio.h>
#include <iostream>
#include <errno.h>
#include <string>
#include "Camera.h"
#include <algorithm>
#include <glui.h>
#include <string.h>
#include "Matrix.h"
#include "Util.h"
#include "Triangle.h"
#include <corona.h>

#define OPENGL_WINDOW 0
#define CLOSE2GL_WINDOW 1
#define EXAMPLE_WINDOW 2

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

float Znear = 0.1f,
Zfar = 100000000.0f;

//
//  Eye point and vectors associated with  
vector3f g_vEye(5.0f, 5.0f, 5.0f);     // Eye Position
vector3f g_vLook(-0.5f, -0.5f, -0.5f); // Look Vector
vector3f g_vUp(0.0f, 1.0f, 0.0f);      // Up Vector
vector3f g_vRight(1.0f, 0.0f, 0.0f);   // Right Vector

extern vector_3d max_pos = { -INFINITY, -INFINITY, -INFINITY };
extern vector_3d min_pos = { INFINITY, INFINITY, INFINITY };
char file_model[255] = "cube_text.in";
int primitives = 0;
int shading = 0;
float attenuation = 0.5;
int texture_mode = 0;
vector_3d centered_position = { 0, 0, 0 }, centered_orientation = { 0, 0, 0 };

float red_color = 0, green_color = 0, blue_color = 0;

int win_id[2];

float data[4][4];
Matrix *viewModel_close2GL = new Matrix(data), *projection_close2GLnew = new Matrix(data), *viewPort_close2GL = new Matrix(data);

Camera cam(viewModel_close2GL, projection_close2GLnew, viewPort_close2GL, win_id, 2);

GLUI_EditText *file;
bool first = true;

float red = 0, green = 0, blue = 0;

int texture = 1;
int perspective_correct = 1;

corona::Image* image = corona::OpenImage("chessboard.jpg");
unsigned char* texture_map = (unsigned char*)image->getPixels();

GLuint texture_name;


// global camera object
//<<<<<<<<<<<<<<<<<<<<<<<< myKeyboard >>>>>>>>>>>>>>>>>>>>>>
void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		// controls for camera
	case 'd':
		cam.slide(0, 0, 0.2);
		break; // slide camera forward
	case 'd' - 32: cam.slide(0, 0, -0.2); break; //slide camera back
	case 's': cam.slide(0, 0.2, 0); break; // slide camera forward
	case 's' - 32: cam.slide(0, -0.2, -0); break; //slide camera back
	case 'a': cam.slide(0.2, 0, 0); break; // slide camera forward
	case 'a' - 32: cam.slide(-0.2, 0, 0); break; //slide camera back
												 // add up/down and left/right controls
	case 'p': cam.pitch(-1.0); break;
	case 'p' - 32: cam.pitch(1.0); break;
	case 'r': cam.roll(-1.0);
		break;
	case 'r' - 32: cam.roll(1.0); break;
	case 'y': cam.yaw(-1.0); break;
	case 'y' - 32: cam.yaw(1.0); break;
		// add roll and yaw controls
	}

	cam.setModelViewMatrix();

	glutSetWindow(win_id[CLOSE2GL_WINDOW]);
	glutPostRedisplay();
	glutSetWindow(win_id[OPENGL_WINDOW]);
	glutPostRedisplay(); // draw it again
}

//*******************************************************************************
//
// close2gl reshape function
//
//*******************************************************************************
void close2glReshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, (GLsizei)w, 0, (GLsizei)h);
}


void openglReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

}

Tri_model* object;

void display_face(const face& face) {
	//GLfloat diffuse[] = { face.diffuse_color.x, face.diffuse_color.y, face.diffuse_color.z };
	GLfloat diffuse[] = { red_color, green_color, blue_color, 1 };
	//glColor3f(face.diffuse_color.x, face.diffuse_color.y, face.diffuse_color.z);
	//glColor3f(red_color, green_color, blue_color);
	if (texture)
		glTexCoord2f(face.v0.texture.x, face.v0.texture.y);
		glTexCoord2f(face.v1.texture.x, face.v1.texture.y);
		glTexCoord2f(face.v2.texture.x, face.v2.texture.y);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glBegin(GL_TRIANGLES);
	glNormal3f(face.v0.normal.x, face.v0.normal.y, face.v0.normal.z);
	glVertex3f(face.v0.pos.x, face.v0.pos.y, face.v0.pos.z);
	glNormal3f(face.v1.normal.x, face.v1.normal.y, face.v1.normal.z);
	glVertex3f(face.v1.pos.x, face.v1.pos.y, face.v1.pos.z);
	glNormal3f(face.v2.normal.x, face.v2.normal.y, face.v2.normal.z);
	glVertex3f(face.v2.pos.x, face.v2.pos.y, face.v2.pos.z);
	glEnd();
}

int zoom = 45, zoom2 = 45;
GLUI_Spinner *zoom_spinner, *zoom_spinner2;
GLUI_Spinner *near_spinner;
GLUI_Spinner *far_spinner;


vector_3d illumination(0, 1, 1), illumination_color(1, 1, 1), ambient(0.3f , 0.3f, 0.3f);
GLUI_Spinner *ambient_light_r;
GLUI_Spinner *ambient_light_g;
GLUI_Spinner *ambient_light_b;

GLUI_Spinner *illumination_r;
GLUI_Spinner *illumination_g;
GLUI_Spinner *illumination_b;

GLUI_Spinner *illumination_dir_x;
GLUI_Spinner *illumination_dir_y;
GLUI_Spinner *illumination_dir_z;

int far_val, near_val;
int lighting = 1, clock = 1;
void display_object() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam.setShape(zoom_spinner->get_int_val(), zoom_spinner2->get_int_val() / zoom_spinner->get_int_val(), near_spinner->get_int_val(), far_spinner->get_int_val());
	cam.setModelViewMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (primitives == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (primitives == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	if (texture) {
		glEnable(GL_TEXTURE_2D);
	}

	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if (clock) {
		glFrontFace(GL_CCW);
	}
	else {
		glFrontFace(GL_CW);
	}

	if (lighting)
	{
		GLfloat diffuse[] = { red, green, blue };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	}
	else
	{
		glColor3f(red, green, blue);
	}

	if (texture) {
		//texture_name = loadBMP_custom("chessboard.bmp");
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_name);
	}

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < object->number_triangles; ++i) {
		display_face(object->faces[i]);
	}
	//display_face(object->faces[5]);
	//display_face(object->faces[1]);

	glEnd();

	if (texture) {
		glDisable(GL_TEXTURE_2D);
	}

	glutSwapBuffers();
}


//*******************************************************************************
// close2gl display function. <render mode> specifies whether you
// should render using points, wireframe or solid mode. <clip mode>
// says whether you should clip or not vertices that fall outside of the
// view frustum.
//
//*******************************************************************************

float *color_buffer;
float **z_buffer;

void set_zero() {
	for (size_t i = 0; i < 500; i++)
	{
		for (size_t j = 0; j < 500; j++)
		{
			z_buffer[i][j] = 1;
			for (size_t k = 0; k < 3; k++)
			{
				color_buffer[3*i + 3 * 500 * j + k] = 100;
			}
		}
	}
}

void display_faces_close2GL(Matrix& proj_view, const face& face, std::vector<Triangle>& triangles) {

	Matrix projected_v0 = proj_view.vector2matrix(Vector3(face.v0.pos.x, face.v0.pos.y, face.v0.pos.z));
	Matrix projected_v1 = proj_view.vector2matrix(Vector3(face.v1.pos.x, face.v1.pos.y, face.v1.pos.z));
	Matrix projected_v2 = proj_view.vector2matrix(Vector3(face.v2.pos.x, face.v2.pos.y, face.v2.pos.z));


	//if (projected_v0.check_vertex_projected() || projected_v1.check_vertex_projected() || projected_v2.check_vertex_projected())
	//	return;
	float w_s[3]{projected_v0.get_data(3, 0), projected_v1.get_data(3, 0), projected_v2.get_data(3, 0) };

	projected_v0.normalize_vertex_project();
	projected_v1.normalize_vertex_project();
	projected_v2.normalize_vertex_project();

	//Matrix world_v0 = viewModel_close2GL->vector2matrix(Vector3(face.v0.pos.x, face.v0.pos.y, face.v0.pos.z));
	//Matrix world_v1 = viewModel_close2GL->vector2matrix(Vector3(face.v1.pos.x, face.v1.pos.y, face.v1.pos.z));
	//Matrix world_v2 = viewModel_close2GL->vector2matrix(Vector3(face.v2.pos.x, face.v2.pos.y, face.v2.pos.z));

	if (projected_v0.check_vertex_projected() || projected_v1.check_vertex_projected() || projected_v2.check_vertex_projected())
		return;

	//world_v0.normalize_vertex_project();
	//world_v1.normalize_vertex_project();
	//world_v2.normalize_vertex_project();


	Matrix viewport_v0 = viewPort_close2GL->multiply_new(projected_v0);
	Matrix viewport_v1 = viewPort_close2GL->multiply_new(projected_v1);
	Matrix viewport_v2 = viewPort_close2GL->multiply_new(projected_v2);


	vertex v0(vector_3d(viewport_v0.get_data(0, 0), viewport_v0.get_data(1, 0), viewport_v0.get_data(2, 0)),
		vector_3d(red, green, blue), face.v0.normal,
		face.v0.pos, w_s[0], face.v0.texture);
	vertex v1(vector_3d(viewport_v1.get_data(0, 0), viewport_v1.get_data(1, 0), viewport_v1.get_data(2, 0)),
		vector_3d(red, green, blue), face.v1.normal,
		face.v1.pos, w_s[1], face.v1.texture);
	vertex v2(vector_3d(viewport_v2.get_data(0, 0), viewport_v2.get_data(1, 0), viewport_v2.get_data(2, 0)),
		vector_3d(red, green, blue), face.v2.normal,
		face.v2.pos, w_s[2], face.v2.texture);
	Triangle* t = new Triangle(v0, v1, v2, z_buffer, color_buffer, 500, 500, face.face_normal);
	triangles.push_back(*t);

}


//TODO: Clipping
void close2glDisplay(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (primitives == 1)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (primitives == 2)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);


	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}

	if (clock) {
		glFrontFace(GL_CCW);
	}
	else {
		glFrontFace(GL_CW);
	}

	if (lighting)
	{
		GLfloat diffuse[] = { red, green, blue };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	}
	else
	{
		glColor3f(red, green, blue);
	}
	glEnable(GL_DEPTH_TEST);

	//glBegin(GL_TRIANGLES);

	Matrix proje_view = projection_close2GLnew->multiply_new(*viewModel_close2GL);
	

	std::vector<Triangle> triangles;
	triangles.reserve(object->number_triangles);
	//set_zero();



	set_zero();


	for (int i = 0; i < object->number_triangles; ++i) {
		display_faces_close2GL(proje_view, object->faces[i], triangles);
	}
	//display_faces_close2GL(proje_view, object->faces[10], triangles);
	//display_faces_close2GL(proje_view, object->faces[11], triangles);
	//display_faces_close2GL(proje_view, object->faces[0], triangles);
	//display_faces_close2GL(proje_view, object->faces[1], triangles);

	for (size_t i = 0; i < object->number_triangles; i++)
	{

		Point3 look = cam.get_eye();
		vector_3d cam_pos = vector_3d(look.x, look.y, look.z);
		triangles[i].set_illumination_settings(ambient, illumination_color, illumination, cam_pos);
		triangles[i].set_mode(primitives);
		triangles[i].set_lighting(lighting);
		triangles[i].set_texture(texture_map, texture, 64, 64);
		triangles[i].set_lighting_type(shading);
		triangles[i].set_attenuation(attenuation);
		triangles[i].set_texture_type(texture_mode);
		if (lighting)
			triangles[i].set_lighting_colors();
		triangles[i].rasterize_triangle(perspective_correct);
	}

	
	//glEnd();
	glDrawPixels(500, 500, GL_RGB, GL_FLOAT, color_buffer);
	
	glutSwapBuffers();
}

int main_window = 1, wireframe = 1, segments = 8;


void myGlutIdle(void)
{

	glutPostRedisplay();
}


void init_close2GL() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(450, 0);
	glutInitWindowSize(500, 500);
	win_id[CLOSE2GL_WINDOW] = glutCreateWindow("Close2GL");
	init_settings(object);
	glutReshapeFunc(close2glReshape);
	glutDisplayFunc(close2glDisplay);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glClearColor(0.3, 0.3, 0.3, 1);
	glEnable(GL_DEPTH_TEST);
}

void init_openGL() {
	float avg_x = (max_pos.x + min_pos.x) / 2;
	float avg_y = (max_pos.y + min_pos.y) / 2;
	float avg_z = (max_pos.z + min_pos.z) / 2;
	float maximum_side = std::max((max_pos.x - min_pos.x), (max_pos.y - min_pos.y));
	g_vEye = { avg_x, avg_y, float((avg_z)+maximum_side) };
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_ALPHA);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(500, 500);
	win_id[OPENGL_WINDOW] = glutCreateWindow("OpenGL");
	init_settings(object);
	glutKeyboardFunc(myKeyboard);
	glutDisplayFunc(display_object);
	glutReshapeFunc(openglReshape);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glShadeModel(GL_SMOOTH);
	cam.set(Point3(avg_x, avg_y, -avg_z + maximum_side), Point3(avg_x, avg_y, avg_z), Vector3(0, 1, 0));
	centered_position = vector_3d(avg_x + 0.1, avg_y + 0.1, -avg_z + maximum_side + 0.1);
	centered_orientation = vector_3d(avg_x, avg_y, avg_z);
	cam.setShape(45.0f, 1, Znear, Zfar);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	//GLuint texture_name = read_and_set_texture("chessboard.jpg");
	texture_name = loadBMP_custom("chessboard.bmp");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_name);
	texture = 1;
}

void reload_file(int control) {
	std::string file_name = file->get_text();
	std::cout << file_name << std::endl;
	object = reading_input_file(file_name.data(), &max_pos, &min_pos, texture);
	float avg_x = (max_pos.x + min_pos.x) / 2;
	float avg_y = (max_pos.y + min_pos.y) / 2;
	float avg_z = (max_pos.z + min_pos.z) / 2;
	float maximum_side = std::max((max_pos.x - min_pos.x), (max_pos.y - min_pos.y));
	g_vEye = { avg_x, avg_y, float((avg_z)+maximum_side) };     // Eye Position
	gluLookAt(avg_x, avg_y, -avg_z + maximum_side, avg_x, avg_y, avg_z, 0, 1, 0);
	cam.set(Point3(avg_x + 0.1, avg_y + 0.1, -avg_z + maximum_side + 0.1), Point3(avg_x, avg_y, avg_z), Vector3(0, 1, 0));
	//cam.set(Point3(0.1, 10, 0.1), Point3(0, 0, -3), Vector3(0, 0, 1));
	cam.setShape(45.0f, 1, Znear, Zfar);
	glutSwapBuffers();
	glutPostRedisplay(); // draw it again
	glClearColor(0.35f, 0.53f, 0.7f, 1.0f);
	max_pos = vector_3d(-INFINITY, -INFINITY, -INFINITY);
	min_pos = vector_3d(INFINITY, INFINITY, INFINITY);
	centered_position = vector_3d(avg_x + 0.1, avg_y + 0.1, -avg_z + maximum_side + 0.1);
	centered_orientation = vector_3d(avg_x, avg_y, avg_z);
}


void follow(int control) {
	cam.set_follow();
}

void reset(int control) {
	cam.set(Point3(centered_position.x, centered_position.y, centered_position.z),
		Point3(centered_orientation.x, centered_orientation.y, centered_orientation.z), Vector3(0, 1, 0));
}

void set_clock(int control) {
	std::cout << clock << std::endl;
	if (clock) {
		glFrontFace(GL_CCW);
	}
	else {
		glFrontFace(GL_CW);
	}
	glutPostRedisplay();
}



void change_near(int control) {
	cam.setShape(45.0f, 1, near_spinner->get_int_val(), cam.getFarDist());
}

void change_far(int control) {
	cam.setShape(45.0f, 1, cam.getNearDist(), far_spinner->get_int_val());
}

GLUI_Spinner *red_spinner, *green_spinner, *blue_spinner;


void change_zoom(int control) {
	cam.setShape(zoom_spinner->get_int_val(), zoom_spinner2->get_int_val() / zoom_spinner->get_int_val(), cam.getNearDist(), cam.getFarDist());
	glutPostRedisplay();
}


void set_lighting(int control) {
	if (lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	glutPostRedisplay();
}

void set_texture(int control) {
	if (!texture) {
		glDisable(GL_TEXTURE_2D);
	}
	else {
		//GLuint texture_name = read_and_set_texture("chessboard.jpg");
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture_name);
	}
	glutPostRedisplay();
}


void change_ambient(int control) {
	ambient = vector_3d(ambient_light_r->get_float_val(), ambient_light_g->get_float_val(), ambient_light_b->get_float_val());
	GLfloat LightAmbient[] = { ambient.x, ambient.y, ambient.z, 1.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, LightAmbient);
}

void change_illumination(int control) {
	illumination_color = vector_3d(illumination_r->get_float_val(), illumination_g->get_float_val(), illumination_b->get_float_val());
}

void change_illumination_dir(int control) {
	illumination = vector_3d(illumination_dir_x->get_float_val(),
		illumination_dir_y->get_float_val(), illumination_dir_z->get_float_val());
	illumination.normalize();
	GLfloat light_position[] = { illumination.x, illumination.y, illumination.z, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

GLUI_Spinner *attenuation_factor_spinner;
void change_attenuation(int control) {
	attenuation = attenuation_factor_spinner->get_float_val();
}

void change_shading(int control) {
	if (shading == 0) {
		glShadeModel(GL_FLAT);
	}
	else {
		glShadeModel(GL_SMOOTH);
	}
	glutPostRedisplay();
}

int main(int argc, char *argv[])
{

	color_buffer = new float[500 * 500 * 3];
	z_buffer = new float *[500];
	for (size_t i = 0; i < 500; i++)
	{
		z_buffer[i] = new float[500];
	}

	object = reading_input_file("cube_text.in", &max_pos, &min_pos, texture);
	glutInit(&argc, argv);
	init_openGL();
	init_close2GL();

	viewPort_close2GL->setViewPort(500, 500);

	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	//GLUI *glui = GLUI_Master.create_glui_subwindow(win_id[OPENGL_WINDOW],
	//	GLUI_SUBWINDOW_RIGHT);

	GLUI *glui = GLUI_Master.create_glui("GLUI");

	file = glui->add_edittext("File path", 1, file_model);
	glui->add_button("Load file", 1, reload_file);

	glui->add_button("Reset Location", 2, reset);

	glui->add_checkbox("Set_follow", &wireframe, 1, follow);

	glui->add_checkbox("Clock wise", &clock, 1, set_clock);

	glui->add_checkbox("Lighting", &lighting, 1, set_lighting);

	zoom_spinner = glui->add_spinner("Zoom", GLUI_SPINNER_INT, &zoom, 1, change_zoom);
	zoom_spinner->set_int_limits(0, 180);

	zoom_spinner2 = glui->add_spinner("Zoom", GLUI_SPINNER_INT, &zoom2, 1, change_zoom);
	zoom_spinner->set_int_limits(0, 180);


	near_spinner =
		glui->add_spinner("Near:", GLUI_SPINNER_INT, &near_val, 1, change_near);
	near_spinner->set_int_limits(1, 10);

	far_spinner =
		glui->add_spinner("Far:", GLUI_SPINNER_INT, &far_val, 1, change_far);
	far_spinner->set_int_limits(10, 2000);

	red_spinner = glui->add_spinner("Red", GLUI_SPINNER_FLOAT, &red, 1);
	red_spinner->set_float_limits(0, 1);
	green_spinner = glui->add_spinner("Green", GLUI_SPINNER_FLOAT, &green, 1);
	green_spinner->set_float_limits(0, 1);
	blue_spinner = glui->add_spinner("Blue", GLUI_SPINNER_FLOAT, &blue, 1);
	blue_spinner->set_float_limits(0, 1);

	glui->add_checkbox("Perspective Correct", &perspective_correct, 1);

	ambient_light_r = glui->add_spinner("Ambient R", GLUI_SPINNER_FLOAT, &ambient.x, 1, change_ambient);
	ambient_light_r->set_float_limits(0, 1);
	ambient_light_g = glui->add_spinner("Ambient G", GLUI_SPINNER_FLOAT, &ambient.y, 1, change_ambient);
	ambient_light_g->set_float_limits(0, 1);
	ambient_light_b = glui->add_spinner("Ambient B", GLUI_SPINNER_FLOAT, &ambient.z, 1, change_ambient);
	ambient_light_b->set_float_limits(0, 1);

	illumination_r = glui->add_spinner("Illumination R", GLUI_SPINNER_FLOAT, &illumination_color.x, 1, change_illumination);
	illumination_r->set_float_limits(0, 1);
	illumination_g = glui->add_spinner("Illumination G", GLUI_SPINNER_FLOAT, &illumination_color.y, 1, change_illumination);
	illumination_g->set_float_limits(0, 1);
	illumination_b = glui->add_spinner("Illumination B", GLUI_SPINNER_FLOAT, &illumination_color.z, 1, change_illumination);
	illumination_b->set_float_limits(0, 1);

	illumination_dir_x = glui->add_spinner("Illumination x", GLUI_SPINNER_FLOAT, &illumination.x, 1, change_illumination_dir);
	illumination_dir_x->set_float_limits(-1000, 1000);
	illumination_dir_y = glui->add_spinner("Illumination y", GLUI_SPINNER_FLOAT, &illumination.y, 1, change_illumination_dir);
	illumination_dir_y->set_float_limits(-1000, 1000);
	illumination_dir_z = glui->add_spinner("Illumination z", GLUI_SPINNER_FLOAT, &illumination.z, 1, change_illumination_dir);
	illumination_dir_z->set_float_limits(-1000, 1000);

	glui->add_column(true);

	GLUI_Panel *obj_panel = glui->add_panel("Primitives");
	GLUI_RadioGroup *group1 = glui->add_radiogroup_to_panel(obj_panel, &primitives, 1);
	glui->add_radiobutton_to_group(group1, "Triangles");
	glui->add_radiobutton_to_group(group1, "Lines");
	glui->add_radiobutton_to_group(group1, "Points");

	glui->add_separator();

	GLUI_Panel *shading_panel = glui->add_panel("Shading");
	GLUI_RadioGroup *group_shading = glui->add_radiogroup_to_panel(shading_panel, &shading, 1, change_shading);
	glui->add_radiobutton_to_group(group_shading, "Gouraud");
	glui->add_radiobutton_to_group(group_shading, "Flat");

	glui->add_separator();

	GLUI_Panel *attennuation_panel = glui->add_panel("Attenuation");
	attenuation_factor_spinner = glui->add_spinner("Constant:", GLUI_SPINNER_FLOAT, &attenuation, 1, change_attenuation);
	attenuation_factor_spinner->set_float_limits(0.0f, 1.0f);

	glui->add_separator();

	GLUI_Panel *texture_panel = glui->add_panel("Texture");
	glui->add_checkbox_to_panel(texture_panel, "Texture",&texture, 1, set_texture);
	GLUI_RadioGroup *texture_group = glui->add_radiogroup_to_panel(texture_panel, &texture_mode, 1);
	glui->add_radiobutton_to_group(texture_group, "Bilinear");
	glui->add_radiobutton_to_group(texture_group, "Nearest Neighbors");
	glui->add_radiobutton_to_group(texture_group, "MipMap");

	//GLUI_Master.set_glutIdleFunc(myGlutIdle);
	//GLUI_Master.set_glutReshapeFunc(openglReshape);

	glutMainLoop();

	return 0;
}
