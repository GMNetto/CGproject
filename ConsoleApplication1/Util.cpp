#include <iostream>
#include <sstream>

#include "Util.h"
#include <glut.h>

#define MAX_MATERIAL_COUNT 10



void init_settings(Tri_model* object)
{
	GLfloat mat_specular[] = { object->specular_color[0].x, object->specular_color[0].y, object->specular_color[0].z, 1.0 };
	GLfloat *mat_shininess = object->shine; //{ 50.0 };
	GLfloat light_position[] = { 0.0, 0.0, 1.0, 1.0 };
	GLfloat ambient[] = { object->ambient_color[0].x, object->ambient_color[0].y, object->ambient_color[0].z, 1.0 };

	GLfloat LightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	GLfloat LightPosition[] = { 5.0f, 5.0f, -10.0f, 1.0f };



	glClearColor(0.5, 0.5, 0.5, 0.0);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_FLAT);
}


void read_segment(FILE* fp, segment* seg, char* segment_name, vector_3d *max_pos, vector_3d *min_pos) {
	std::stringstream ss;
	ss << segment_name << " %f %f %f %f %f %f %d\n";
	std::string s = ss.str();
	fscanf(fp, s.data(),
		&(seg->pos.x), &(seg->pos.y), &(seg->pos.z),
		&(seg->normal.x), &(seg->normal.y), &(seg->normal.z),
		&(seg->color_index));
	if (seg->pos.x > max_pos->x) max_pos->x = seg->pos.x;
	if (seg->pos.y > max_pos->y) max_pos->y = seg->pos.y;
	if (seg->pos.z > max_pos->z) max_pos->z = seg->pos.z;
	if (seg->pos.x < min_pos->x) min_pos->x = seg->pos.x;
	if (seg->pos.y < min_pos->y) min_pos->y = seg->pos.y;
	if (seg->pos.z < min_pos->z) min_pos->z = seg->pos.z;
}

Tri_model* reading_input_file(const char *FileName, vector_3d *max_pos, vector_3d *min_pos)
{
	Tri_model *object = new Tri_model;
	vector_3d ambient[MAX_MATERIAL_COUNT],
		diffuse[MAX_MATERIAL_COUNT],
		specular[MAX_MATERIAL_COUNT];
	float shine[MAX_MATERIAL_COUNT];
	int material_count,
		color_index[3], i;
	char  ch;
	FILE* fp = fopen(FileName, "r");
	if (fp == NULL) {
		printf("ERROR: unable to open TriObj[%s]!\n", FileName);
		exit(1);
	}
	fscanf(fp, "%c", &ch);
	while (ch != '\n')
		// skip the first line – object’s name
		fscanf(fp, "%c", &ch);
	//
	int NumTris;
	fscanf(fp, "# triangles = %d\n", &object->number_triangles);
	object->faces = new face[object->number_triangles]();
	// read # of triangles 
	fscanf(fp, "Material count = %d\n", &object->material_count);    // read material count 
																	 //
	object->ambient_color = new vector_3d[object->material_count];
	object->diffuse_color = new vector_3d[object->material_count];
	object->specular_color = new vector_3d[object->material_count];
	object->shine = new float[object->material_count];
	for (i = 0; i<object->material_count; i++) {
		fscanf(fp, "ambient color %f %f %f\n", &(object->ambient_color[i].x), &(object->ambient_color[i].y), &(object->ambient_color[i].z));
		float a = object->ambient_color[i].x;
		fscanf(fp, "diffuse color %f %f %f\n", &(object->diffuse_color[i].x), &(object->diffuse_color[i].y), &(object->diffuse_color[i].z));
		fscanf(fp, "specular color %f %f %f\n", &(object->specular_color[i].x), &(object->specular_color[i].y), &(object->specular_color[i].z));
		fscanf(fp, "material shine %f\n", &(object->shine[i]));
	}
	// 
	fscanf(fp, "%c", &ch);
	while (ch != '\n')
		// skip documentation line 
		fscanf(fp, "%c", &ch);
	// 
	//  allocate triangles for tri model 
	//
	printf("Reading in %s (%d triangles). . .\n", FileName, object->number_triangles);
	// 
	for (i = 0; i<object->number_triangles; i++)
		// read triangles 
	{
		face* current_face = &object->faces[i];
		read_segment(fp, &(current_face->v0), "v0", max_pos, min_pos);
		read_segment(fp, &(current_face->v1), "v1", max_pos, min_pos);
		read_segment(fp, &(current_face->v2), "v2", max_pos, min_pos);
		fscanf(fp, "face normal %f %f %f\n", &(current_face->face_normal.x), &(current_face->face_normal.y),
			&(current_face->face_normal.z));
		// 
		current_face->diffuse_color.x = (unsigned char)(int)(255 * (object->diffuse_color[current_face->v0.color_index].x));
		current_face->diffuse_color.y = (unsigned char)(int)(255 * (object->diffuse_color[current_face->v1.color_index].y));
		current_face->diffuse_color.z = (unsigned char)(int)(255 * (object->diffuse_color[current_face->v2.color_index].z));


	}
	fclose(fp);
	return object;
}
