#include <iostream>
#include <sstream>

#include "Util.h"
#include <glut.h>
#include <corona.h>

#define MAX_MATERIAL_COUNT 10

GLuint read_and_set_texture(std::string texture_path) {
	corona::Image* image = corona::OpenImage("chessboard.jpg");
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
	// setting mip map pyramid
	//
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->getWidth(), image->getHeight(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, image->getPixels());
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->getWidth(), image->getHeight(),
		GL_RGB, GL_UNSIGNED_BYTE, image->getPixels());
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	return texture_id;
}


GLuint loadBMP_custom(const char * imagepath) {

	printf("Reading image %s\n", imagepath);

	// Data read from the header of the BMP file
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width, height;
	// Actual RGB data
	unsigned char * data;

	// Open the file
	FILE * file = fopen(imagepath, "rb");
	if (!file) { printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", imagepath); getchar(); return 0; }

	// Read the header, i.e. the 54 first bytes

	// If less than 54 bytes are read, problem
	if (fread(header, 1, 54, file) != 54) {
		printf("Not a correct BMP file\n");
		return 0;
	}
	// A BMP files always begins with "BM"
	if (header[0] != 'B' || header[1] != 'M') {
		printf("Not a correct BMP file\n");
		return 0;
	}
	// Make sure this is a 24bpp file
	if (*(int*)&(header[0x1E]) != 0) { printf("Not a correct BMP file\n");    return 0; }
	if (*(int*)&(header[0x1C]) != 24) { printf("Not a correct BMP file\n");    return 0; }

	// Read the information about the image
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way

										 // Create a buffer
	data = new unsigned char[imageSize];

	// Read the actual data from the file into the buffer
	fread(data, 1, imageSize, file);

	// Everything is in memory now, the file wan be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	// OpenGL has now copied the data. Free our own version

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	//glGenerateMipmap(GL_TEXTURE_2D);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE,data);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height,
	//	GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	delete[] data;
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//
	// setting mip map pyramid
	//
	// Return the ID of the texture we just created
	return textureID;
}


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


void read_segment(FILE* fp, segment* seg, char* segment_name, vector_3d *max_pos, vector_3d *min_pos, bool texture) {
	std::stringstream ss;
	if (!texture) {
		ss << segment_name << " %f %f %f %f %f %f %d\n";
		std::string s = ss.str();
		fscanf(fp, s.data(),
			&(seg->pos.x), &(seg->pos.y), &(seg->pos.z),
			&(seg->normal.x), &(seg->normal.y), &(seg->normal.z),
			&(seg->color_index));
	}
	else {
		ss << segment_name << " %f %f %f %f %f %f %d %f %f\n";
		std::string s = ss.str();
		int a = fscanf(fp, s.data(),
			&(seg->pos.x), &(seg->pos.y), &(seg->pos.z),
			&(seg->normal.x), &(seg->normal.y), &(seg->normal.z),
			&(seg->color_index), &(seg->texture.x), &(seg->texture.y));
	}
	if (seg->pos.x > max_pos->x) max_pos->x = seg->pos.x;
	if (seg->pos.y > max_pos->y) max_pos->y = seg->pos.y;
	if (seg->pos.z > max_pos->z) max_pos->z = seg->pos.z;
	if (seg->pos.x < min_pos->x) min_pos->x = seg->pos.x;
	if (seg->pos.y < min_pos->y) min_pos->y = seg->pos.y;
	if (seg->pos.z < min_pos->z) min_pos->z = seg->pos.z;
}

Tri_model* reading_input_file(const char *FileName, vector_3d *max_pos, vector_3d *min_pos, bool texture)
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
		read_segment(fp, &(current_face->v0), "v0", max_pos, min_pos, texture);
		read_segment(fp, &(current_face->v1), "v1", max_pos, min_pos, texture);
		read_segment(fp, &(current_face->v2), "v2", max_pos, min_pos, texture);
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
