#define GLM_FORCE_RADIANS

/* OpenGL includes */
#include <GL/glew.h>
#include <GL/freeglut.h>

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <math.h>

// Include GLM
// glm::vec3, glm::vec4, glm::ivec4, glm::mat4
#include "glm.hpp"
// glm::translate, glm::rotate, glm::scale
#include "gtc/matrix_transform.hpp"
// glm::value_ptr
#include "gtc/type_ptr.hpp"

//from opengl-tutorial.org
#include "objloader.h"
#include "shader.h"
#include "texture.h"

using namespace glm;

/* Strings for loading and storing shader code */
static const char* VertexShaderString;
static const char* FragmentShaderString;

glm::mat4 ViewMatrixGLM;
glm::mat4 ProjectionMatrixGLM;
glm::mat4 MVP;
glm::mat4 ModelMatrixTemp;

GLuint TextureStandard;
GLuint TextureBricks;
GLuint TextureCrate;
GLuint TextureTiles;
GLuint TextureSky;
GLuint TextureGrass;
GLuint TextureID;
GLuint TextureBillboard;

GLuint ShaderProgram;
GLuint programID;
GLuint LightID;
GLuint LightColorID;
GLuint LightPowerID;

//rgb values for light
float lightR = 1.0f;
float lightG = 1.0f;
float lightB = 1.0f;

//values for light power
float lightPower = 50.0f;

GLuint VertexArrayID;

// Get a handle for our "MVP" uniform
GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;

float rotateCamera = 0;
float rotateAngleObjects = 0;
float rotateAngleBar1 = 0;
float rotateAngleBar2 = 0;
float speedFactor = 1;

float timer;
float uv_offset = 0.125;
int billboard_tile_counter = 1;
bool change_row = false;

std::vector<glm::vec3> vertices;
std::vector<glm::vec2> uvs;
std::vector<glm::vec3> normals;

std::vector<glm::vec3> vertices_cube;
std::vector<glm::vec2> uvs_cube;
std::vector<glm::vec3> normals_cube;

std::vector<glm::vec3> vertices_pyramide;
std::vector<glm::vec2> uvs_pyramide;
std::vector<glm::vec3> normals_pyramide;

std::vector<glm::vec3> vertices_trapez;
std::vector<glm::vec2> uvs_trapez;
std::vector<glm::vec3> normals_trapez;

std::vector<glm::vec3> vertices_bar1;
std::vector<glm::vec2> uvs_bar1;
std::vector<glm::vec3> normals_bar1;

std::vector<glm::vec3> vertices_bar2;
std::vector<glm::vec2> uvs_bar2;
std::vector<glm::vec3> normals_bar2;
	
std::vector<glm::vec3> vertices_billboard;
std::vector<glm::vec2> uvs_billboard;
std::vector<glm::vec3> normals_billboard;

GLuint vertexbuffer;
GLuint uvbuffer;
GLuint normalbuffer;

GLuint vertexbuffer_cube;
GLuint uvbuffer_cube;
GLuint normalbuffer_cube;

GLuint vertexbuffer_pyramide;
GLuint uvbuffer_pyramide;
GLuint normalbuffer_pyramide;

GLuint vertexbuffer_trapez;
GLuint uvbuffer_trapez;
GLuint normalbuffer_trapez;

GLuint vertexbuffer_bar1;
GLuint uvbuffer_bar1;
GLuint normalbuffer_bar1;

GLuint vertexbuffer_bar2;
GLuint uvbuffer_bar2;
GLuint normalbuffer_bar2;

GLuint vertexbuffer_billboard;
GLuint uvbuffer_billboard;
GLuint normalbuffer_billboard;	

void drawObject(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer, glm::mat4 ModelMatrix, GLuint verticesSize) {
	glPushMatrix();

	// Use our shader
	glUseProgram(programID);

	// Compute the MVP matrix from keyboard and mouse input
	//glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(1.0f, 0.0f, 0.f));

	//ViewMatrixGLM = glm::rotate(ViewMatrixGLM, angle, glm::vec3(-1.0f, 0.0f, 0.0f));		
	glm::mat4 MVP = ProjectionMatrixGLM * ViewMatrixGLM * ModelMatrix;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrixGLM[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// 3rd attribute buffer : normals
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
		);

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLES, 0, verticesSize);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);


	glPopMatrix();
}

mat4 getBillboardTransform(vec3 position, vec3 cameraPos, vec3 cameraUp) {
	vec3 look = normalize(cameraPos - position);
	vec3 right = cross(cameraUp, look);
	vec3 up2 = cross(look, right);
	mat4 transform;
	transform[0] = vec4(right, 0);
	transform[1] = vec4(up2, 0);
	transform[2] = vec4(look, 0);
	transform[3] = vec4(position, 1);
	return transform;
}

void updateUVTexture(std::vector<glm::vec2, std::allocator<glm::vec2>>* uvs_tex) {
	for(int i = 0; i < uvs_tex->size(); i++) {
		(*uvs_tex)[i].x += uv_offset;
		if(change_row) {
			(*uvs_tex)[i].y += uv_offset;
		}
	}
	change_row = false;
	billboard_tile_counter++;
	if(billboard_tile_counter%8 == 0){
		change_row = true;
	}

}

void Display() {

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//glm::mat4 lightTrans = glm::mat4
	glm::mat4 lightRot = glm::mat4(1.0f);
	glm::mat4 lightPosition = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, -50.0f)
		);

	glUniform3f(LightID, 0, 0, 0);
	glUniform3f(LightColorID, lightR, lightG, lightB);
	glUniform1f(LightPowerID, lightPower);

	//ProjectionMatrixGLM = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	//ViewMatrixGLM = glm::make_mat4(ViewMatrix);
	ViewMatrixGLM = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(0.0f, 0.0f, -25.0f));
	ViewMatrixGLM = glm::rotate(ViewMatrixGLM, 3.14f / 9.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	ViewMatrixGLM = glm::rotate(ViewMatrixGLM, rotateCamera, glm::vec3(0.0f, 1.0f, 0.0f));

	//Object1
	glBindTexture(GL_TEXTURE_2D, TextureCrate);
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(5.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(3.0f, -5.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleObjects, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, -3.14f / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(vertexbuffer_cube, uvbuffer_cube, normalbuffer_cube, ModelMatrixTemp, vertices_cube.size());

	//Object2
	glBindTexture(GL_TEXTURE_2D, TextureSky);
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(5.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(-3.0f, -5.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleObjects, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, -3.14f / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(vertexbuffer_pyramide, uvbuffer_pyramide, normalbuffer_pyramide, ModelMatrixTemp, vertices_pyramide.size());

	//Object3
	glBindTexture(GL_TEXTURE_2D, TextureTiles);
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(-5.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(3.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleObjects, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, -3.14f / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(vertexbuffer_trapez, uvbuffer_trapez, normalbuffer_trapez, ModelMatrixTemp, vertices_trapez.size());

	//Object4
	glBindTexture(GL_TEXTURE_2D, TextureBricks);
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(-5.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(-3.0f, 0.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleObjects, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, -3.14f / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(vertexbuffer_cube, uvbuffer_cube, normalbuffer_cube, ModelMatrixTemp, vertices_cube.size());


	//bar1
	glBindTexture(GL_TEXTURE_2D, TextureStandard);
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(0.0f, 10.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	drawObject(vertexbuffer_bar1, uvbuffer_bar1, normalbuffer_bar1, ModelMatrixTemp, vertices_bar1.size());

	//bar23
	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(-5.0f, 6.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	drawObject(vertexbuffer_bar2, uvbuffer_bar2, normalbuffer_bar2, ModelMatrixTemp, vertices_bar2.size());

	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar1, glm::vec3(0.0f, 1.0f, 0.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(5.0f, 3.0f, 0.0f));
	ModelMatrixTemp = glm::rotate(ModelMatrixTemp, rotateAngleBar2, glm::vec3(0.0f, 1.0f, 0.0f));
	drawObject(vertexbuffer_bar2, uvbuffer_bar2, normalbuffer_bar2, ModelMatrixTemp, vertices_bar2.size());

	//floor	
	glBindTexture(GL_TEXTURE_2D, TextureGrass);

	ModelMatrixTemp = glm::mat4(1.0);
	ModelMatrixTemp = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(10.0f, 0.1f, 10.0f));
	ModelMatrixTemp = glm::translate(ModelMatrixTemp, glm::vec3(0.0f, -100.0f, 0.0f));
	drawObject(vertexbuffer_cube, uvbuffer_cube, normalbuffer_cube, ModelMatrixTemp, vertices_cube.size());

	//billboard
	updateUVTexture(&uvs_billboard);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_billboard);
	glBufferData(GL_ARRAY_BUFFER, uvs_billboard.size() * sizeof(glm::vec2), &uvs_billboard[0], GL_STATIC_DRAW);
	glBindTexture(GL_TEXTURE_2D, TextureBillboard);
	glm::mat4 inverseView = glm::inverse(ViewMatrixGLM);
	//glm::vec3 camPos = glm::vec3(inverseView[3].x, inverseView[3].y, inverseView[3].z);
	glm::vec3 camPos = glm::vec3(inverseView[3].x, 0.0f, inverseView[3].z); // don't rotate on y axis
	glm::vec3 camUp = glm::vec3(ViewMatrixGLM[0].y, ViewMatrixGLM[1].y, ViewMatrixGLM[2].y);
	ModelMatrixTemp = getBillboardTransform(vec3(0.0f, -8.0f, 0.0f), camPos, camUp);
	drawObject(vertexbuffer_billboard, uvbuffer_billboard, normalbuffer_billboard, ModelMatrixTemp, vertices_billboard.size());

	/* Swap between front and back buffer */
	glutSwapBuffers();
}


void OnIdle() {
	float time = glutGet(GLUT_ELAPSED_TIME);
	float timeDiff = time - timer;
	timer = time;

	rotateCamera += 0.0001 * speedFactor * timeDiff;
	rotateAngleObjects -= 0.0025 * speedFactor * timeDiff;
	rotateAngleBar2 += 0.00025 * speedFactor * timeDiff;
	rotateAngleBar1 += 0.001 * speedFactor * timeDiff;


	glutPostRedisplay();
}

void Initialize(void) {
	/* Set viewing transform */
	ProjectionMatrixGLM = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	//texture
	TextureStandard = loadDDS("objects/standard.dds");
	TextureBricks = loadDDS("objects/bricks.dds");
	TextureCrate = loadDDS("objects/crate.dds");
	TextureTiles = loadDDS("objects/tiles.dds");
	TextureSky = loadDDS("objects/sky.dds");
	TextureGrass = loadDDS("objects/grass.dds");
	TextureBillboard = loadBMP_custom("objects/uvtemplate.bmp");
	TextureBillboard = loadDDS("objects/Flame64SeqLayout.dds");

	TextureID = glGetUniformLocation(programID, "myTextureSampler");

	//Load cube
	loadOBJ("objects/cube.obj", vertices_cube, uvs_cube, normals_cube);
	glGenBuffers(1, &vertexbuffer_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_cube);
	glBufferData(GL_ARRAY_BUFFER, vertices_cube.size() * sizeof(glm::vec3), &vertices_cube[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_cube);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_cube);
	glBufferData(GL_ARRAY_BUFFER, uvs_cube.size() * sizeof(glm::vec2), &uvs_cube[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer_cube);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_cube);
	glBufferData(GL_ARRAY_BUFFER, normals_cube.size() * sizeof(glm::vec3), &normals_cube[0], GL_STATIC_DRAW);

	//Load pyramide
	loadOBJ("objects/pyramide.obj", vertices_pyramide, uvs_pyramide, normals_pyramide);
	glGenBuffers(1, &vertexbuffer_pyramide);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_pyramide);
	glBufferData(GL_ARRAY_BUFFER, vertices_pyramide.size() * sizeof(glm::vec3), &vertices_pyramide[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_pyramide);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_pyramide);
	glBufferData(GL_ARRAY_BUFFER, uvs_pyramide.size() * sizeof(glm::vec2), &uvs_pyramide[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer_pyramide);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_pyramide);
	glBufferData(GL_ARRAY_BUFFER, normals_pyramide.size() * sizeof(glm::vec3), &normals_pyramide[0], GL_STATIC_DRAW);


	//Load trapez
	loadOBJ("objects/trapez.obj", vertices_trapez, uvs_trapez, normals_trapez);
	glGenBuffers(1, &vertexbuffer_trapez);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_trapez);
	glBufferData(GL_ARRAY_BUFFER, vertices_trapez.size() * sizeof(glm::vec3), &vertices_trapez[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_trapez);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_trapez);
	glBufferData(GL_ARRAY_BUFFER, uvs_trapez.size() * sizeof(glm::vec2), &uvs_trapez[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer_trapez);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_trapez);
	glBufferData(GL_ARRAY_BUFFER, normals_trapez.size() * sizeof(glm::vec3), &normals_trapez[0], GL_STATIC_DRAW);

	//Load bar1
	loadOBJ("objects/bar1.obj", vertices_bar1, uvs_bar1, normals_bar1);
	glGenBuffers(1, &vertexbuffer_bar1);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_bar1);
	glBufferData(GL_ARRAY_BUFFER, vertices_bar1.size() * sizeof(glm::vec3), &vertices_bar1[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_bar1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_bar1);
	glBufferData(GL_ARRAY_BUFFER, uvs_bar1.size() * sizeof(glm::vec2), &uvs_bar1[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer_bar1);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_bar1);
	glBufferData(GL_ARRAY_BUFFER, normals_bar1.size() * sizeof(glm::vec3), &normals_bar1[0], GL_STATIC_DRAW);

	//Load bar2
	loadOBJ("objects/bar2.obj", vertices_bar2, uvs_bar2, normals_bar2);
	glGenBuffers(1, &vertexbuffer_bar2);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_bar2);
	glBufferData(GL_ARRAY_BUFFER, vertices_bar2.size() * sizeof(glm::vec3), &vertices_bar2[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_bar2);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_bar2);
	glBufferData(GL_ARRAY_BUFFER, uvs_bar2.size() * sizeof(glm::vec2), &uvs_bar2[0], GL_STATIC_DRAW);

	glGenBuffers(1, &normalbuffer_bar2);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_bar2);
	glBufferData(GL_ARRAY_BUFFER, normals_bar2.size() * sizeof(glm::vec3), &normals_bar2[0], GL_STATIC_DRAW);

	//Load billboard
	loadOBJ("objects/billboard.obj", vertices_billboard, uvs_billboard, normals_billboard);
	glGenBuffers(1, &vertexbuffer_billboard);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_billboard);
	glBufferData(GL_ARRAY_BUFFER, vertices_billboard.size() * sizeof(glm::vec3), &vertices_billboard[0], GL_STATIC_DRAW);

	glGenBuffers(1, &uvbuffer_billboard);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_billboard);
	glBufferData(GL_ARRAY_BUFFER, uvs_billboard.size() * sizeof(glm::vec2), &uvs_billboard[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &normalbuffer_billboard);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer_billboard);
	glBufferData(GL_ARRAY_BUFFER, normals_billboard.size() * sizeof(glm::vec3), &normals_billboard[0], GL_STATIC_DRAW);

}

void Keyboard(unsigned char key, int x, int y) {

	switch (key) {
		//change rgb values for light;				
	case 'u':
		if (lightR <= 0.9)
			lightR += 0.1;

		break;
	case 'i':
		if (lightG <= 0.9)
			lightG += 0.1;
		break;
	case 'o':
		if (lightB <= 0.9)
			lightB += 0.1;
		break;
	case 'j':
		if (lightR >= 0.09)
			lightR -= 0.1;
		break;
	case 'k':
		if (lightG >= 0.09)
			lightG -= 0.1;
		break;
	case 'l':
		if (lightB >= 0.09)
			lightB -= 0.1;
		break;
		//change color power	
	case 'z':
		lightPower += 1;
		break;
	case 'h':
		lightPower -= 1;
		break;
	case 't':
		speedFactor += 0.1;
		break;
	case 'g':
		speedFactor -= 0.1;
		break;

	}
	printf("%f, %f, %f, Power: %f\n", lightR, lightG, lightB, lightPower);
}

int main(int argc, char** argv) {
	/* Initialize GLUT; set double buffered window and RGBA color model */
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 1000);
	glutInitWindowPosition(400, 400);
	glutCreateWindow("Mobile");

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}


	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);


	/* Set background (clear) color to blue */
	glClearColor(0.0, 0.0, 0.35, 0.0);

	Initialize();

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	LightColorID = glGetUniformLocation(programID, "LightColor");
	LightPowerID = glGetUniformLocation(programID, "LightPower");

	/* Specify callback functions;enter GLUT event processing loop,
	 * handing control over to GLUT */
	glutIdleFunc(OnIdle);
	glutKeyboardFunc(Keyboard);
	//glutSpecialFunc(SpecialKeyboard);
	glutDisplayFunc(Display);
	glutMainLoop();

	/* ISO C requires main to return int */
	return 0;
}

