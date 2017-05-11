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
char file_model[255] = "cube.in";
int primitives = 0;
vector_3d centered_position = { 0, 0, 0 }, centered_orientation = { 0, 0, 0 };

float red_color = 0, green_color = 0, blue_color = 0;

int win_id[2];

float data[4][4];
Matrix *viewModel_close2GL = new Matrix(data), *projection_close2GLnew = new Matrix(data), *viewPort_close2GL = new Matrix(data);

Camera cam(viewModel_close2GL, projection_close2GLnew, viewPort_close2GL, win_id, 2);

GLUI_EditText *file;
bool first = true;

float red = 0, green = 0, blue = 0;



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
	
	glutSetWindow(win_id[CLOSE2GL_WINDOW]);
	cam.setModelViewMatrix();
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
	glutSetWindow(win_id[CLOSE2GL_WINDOW]);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 1, 0, 1);
	glMatrixMode(GL_MODELVIEW);
	viewPort_close2GL->setViewPort(1, 1);
	glutSetWindow(win_id[OPENGL_WINDOW]);
}


//***********************************************
//
//  OpenGL Reshape function
//
//  Sets the viewport and the projection matrix
//  
//***********************************************
void openglReshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

}

Tri_model* object;

void display_face(const face& face) {
	//GLfloat diffuse[] = { face.diffuse_color.x, face.diffuse_color.y, face.diffuse_color.z };
	GLfloat diffuse[] = { red_color, green_color, blue_color, 1 };
	//glColor3f(face.diffuse_color.x, face.diffuse_color.y, face.diffuse_color.z);
	glColor3f(red_color, green_color, blue_color);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glBegin(GL_TRIANGLES);
	glNormal3f(face.face_normal.x, face.face_normal.y, face.face_normal.z);
	glVertex3f(face.v0.pos.x, face.v0.pos.y, face.v0.pos.z);
	glVertex3f(face.v1.pos.x, face.v1.pos.y, face.v1.pos.z);
	glVertex3f(face.v2.pos.x, face.v2.pos.y, face.v2.pos.z);
	glEnd();
}

int zoom = 45;
GLUI_Spinner *zoom_spinner;
GLUI_Spinner *near_spinner;
GLUI_Spinner *far_spinner;

int far_val, near_val;
int lighting = 1, clock = 1;
void display_object() {
	//glutSetWindow(win_id[OPENGL_WINDOW]);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam.setShape(zoom_spinner->get_int_val(), 1, near_spinner->get_int_val(), far_spinner->get_int_val());


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
	glBegin(GL_TRIANGLES);

	for (int i = 0; i < object->number_triangles; ++i) {
		display_face(object->faces[i]);
	}

	glEnd();

	glutSwapBuffers();
}


//*******************************************************************************
// close2gl display function. <render mode> specifies whether you
// should render using points, wireframe or solid mode. <clip mode>
// says whether you should clip or not vertices that fall outside of the
// view frustum.
//
//*******************************************************************************

void display_faces_close2GL(Matrix& proj_view, const face& face) {
	Matrix projected_v0 = proj_view.vector2matrix(Vector3(face.v0.pos.x, face.v0.pos.y, face.v0.pos.z));
	Matrix projected_v1 = proj_view.vector2matrix(Vector3(face.v1.pos.x, face.v1.pos.y, face.v1.pos.z));
	Matrix projected_v2 = proj_view.vector2matrix(Vector3(face.v2.pos.x, face.v2.pos.y, face.v2.pos.z));

	if (projected_v0.check_vertex_projected() || projected_v1.check_vertex_projected() || projected_v2.check_vertex_projected())
		return;

	projected_v0.normalize_vertex_project();
	projected_v1.normalize_vertex_project();
	projected_v2.normalize_vertex_project();

	Matrix viewport_v0 = viewPort_close2GL->multiply_new(projected_v0);
	Matrix viewport_v1 = viewPort_close2GL->multiply_new(projected_v1);
	Matrix viewport_v2 = viewPort_close2GL->multiply_new(projected_v2);

	//Clipping
	//(face.face_normal.x, face.face_normal.y, face.face_normal.z);
	glVertex2f(viewport_v0.get_data(0, 0), viewport_v0.get_data(1, 0));
	glVertex2f(viewport_v1.get_data(0, 0), viewport_v1.get_data(1, 0));
	glVertex2f(viewport_v2.get_data(0, 0), viewport_v2.get_data(1, 0));

}

void close2glDisplay(void)
{

	glutSetWindow(win_id[CLOSE2GL_WINDOW]);

	cam.setShape(zoom_spinner->get_int_val(), 1, near_spinner->get_int_val(), far_spinner->get_int_val());


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

	glBegin(GL_TRIANGLES);

	Point3 eye = cam.get_eye();
	Matrix proje_view = projection_close2GLnew->multiply_new(*viewModel_close2GL);
	//proje_view.transpose();
	for (int i = 0; i < object->number_triangles; ++i) {
		display_faces_close2GL(proje_view, object->faces[i]);
	}


	glEnd();

	glutSwapBuffers();
}

int main_window = 1, wireframe = 0, segments = 8;


void myGlutIdle(void)
{

	glutPostRedisplay();
}


void init_close2GL() {
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(450, 0);
	glutInitWindowSize(500, 500);
	win_id[CLOSE2GL_WINDOW] = glutCreateWindow("Close2GL");
	init_settings(object);
	glutDisplayFunc(close2glDisplay);
	glutReshapeFunc(close2glReshape);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_ALPHA);
	glClearColor(0.3, 0.3, 0.3, 1);
	glEnable(GL_DEPTH_TEST);

	projection_close2GLnew->setProjection(cam.getNearDist(), cam.getFarDist(), 45.0f, 1);

	viewPort_close2GL->setViewPort(1, 1);
	cam.setModelViewMatrix();
	cam.setShape(45.0f, 1, Znear, Zfar);

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
	cam.set(Point3(avg_x + 0.1, avg_y + 0.1, -avg_z + maximum_side + 0.1), Point3(avg_x, avg_y, avg_z), Vector3(0, 1, 0));
	centered_position = vector_3d(avg_x + 0.1, avg_y + 0.1, -avg_z + maximum_side + 0.1);
	centered_orientation = vector_3d(avg_x, avg_y, avg_z);
	cam.setShape(45.0f,1, Znear, Zfar);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

void reload_file(int control) {
	std::string file_name = file->get_text();
	std::cout << file_name << std::endl;
	object = reading_input_file(file_name.data(), &max_pos, &min_pos);
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
	cam.setShape(zoom_spinner->get_int_val(), 1, cam.getNearDist(), cam.getFarDist());
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





int main(int argc, char *argv[])
{
	object = reading_input_file("cube.in", &max_pos, &min_pos);
	glutInit(&argc, argv);
	init_openGL();
	init_close2GL();
	
	/****************************************/
	/*         Here's the GLUI code         */
	/****************************************/

	GLUI *glui = GLUI_Master.create_glui_subwindow(win_id[OPENGL_WINDOW],
		GLUI_SUBWINDOW_RIGHT);

	file = glui->add_edittext("File path", 1, file_model);
	glui->add_button("Load file", 1, reload_file);

	glui->add_button("Reset Location", 2, reset);

	glui->add_checkbox("Set_follow", &wireframe, 1, follow);

	glui->add_checkbox("Clock wise", &clock, 1, set_clock);

	glui->add_checkbox("Lighting", &lighting, 1, set_lighting);

	zoom_spinner = glui->add_spinner("Zoom", GLUI_SPINNER_INT, &zoom, 1, change_zoom);
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


	GLUI_Panel *obj_panel = glui->add_panel("Primitives");
	GLUI_RadioGroup *group1 = glui->add_radiogroup_to_panel(obj_panel, &primitives, 1);
	glui->add_radiobutton_to_group(group1, "Triangles");
	glui->add_radiobutton_to_group(group1, "Lines");
	glui->add_radiobutton_to_group(group1, "Points");


	GLUI_Master.set_glutIdleFunc(myGlutIdle);
	GLUI_Master.set_glutReshapeFunc(openglReshape);

	glutMainLoop();

	return 0;
}
