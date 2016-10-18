

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <cmath>
#include <stdexcept>
#include <iostream>

// Defines maximum stack capacity.
#define STACK_CAP 16
// Defines pi for converting angles.
#define PI 3.14159265359

// Structure for the matrix stack, top specifies current top position on the stack, initially zero (which means one matrix in the stack).
struct matrix_stack
{
	GLdouble m[STACK_CAP][16];
	int top;
};

// Define a macro for retrieving current matrix from top of current stack.
#define current_matrix (current_stack->m[current_stack->top])

// Identity matrix constant.
const GLdouble identity[16] =
{ 1, 0, 0, 0,
 0, 1, 0, 0,
 0, 0, 1, 0,
 0, 0, 0, 1 };

// The model view matrix stack.
struct matrix_stack model_view = { {{0}}, 0 };
// The projection matrix stack.
struct matrix_stack projection = { {{0}}, 0 };
// The current stack pointer that specifies the matrix mode.
struct matrix_stack *current_stack = &model_view;

// Multiplies current matrix with matrix b, put the result in current matrix.
// current = current * b
void matrix_multiply(const GLdouble *b)
{
	GLdouble* temp;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp = current_matrix;
			current_matrix[4 * i + j] = 0;
			for (int k = 0; k < 4; k++) {
				current_matrix[4 * i + j] += temp[4 * i + k] * temp[4 * k + j];
			}
		}
	}
}

// Calculates cross product of b and c, put the result in a
// a = b x c.
void cross_product(GLdouble *ax, GLdouble *ay, GLdouble *az,
	GLdouble bx, GLdouble by, GLdouble bz,
	GLdouble cx, GLdouble cy, GLdouble cz)
{
	ax = new GLdouble(by*cz - cy*bz);
	ay = new GLdouble(bz*cx - cz*bx);
	az = new GLdouble(bx*cy - cx*by);
}

// Normalizes vector (x, y, z).
void normalize(GLdouble *x, GLdouble *y, GLdouble *z)
{
	float length = sqrt(*x * *x + *y * *y + *z * * z);
	x = new GLdouble(*x / length);
	y = new GLdouble(*y / length);
	z = new GLdouble(*z / length);
}

// Switches matrix mode by changing the current stack pointer.
void I_my_glMatrixMode(GLenum mode)
{
	switch (mode) {
	case GL_MODELVIEW:
		current_stack = &model_view;
	case GL_PROJECTION:
		current_stack = &projection;
	default:
		break;
	}
}

// Overwrites current matrix with identity matrix.
void I_my_glLoadIdentity(void)
{
	for (int i = 0; i < 16; i++) {
		current_matrix[i] = identity[i];
	}
}

// Pushes current matrix onto current stack, report error if the stack is already full.
void I_my_glPushMatrix(void)
{
	try {
		if (sizeof(current_stack) == STACK_CAP) {
			throw;
		}
		else {
			current_stack->top += 1;
			*current_stack->m[current_stack->top] = *current_matrix;
		}
	}
	catch (int e) {
		std::cerr << "Error: current stack is full" << std::endl;
	}
}

// Pops current matrix from current stack, report error if the stack has only one matrix left.
void I_my_glPopMatrix(void)
{
	try {
		if (sizeof(current_stack) == 1) {
			throw;
		}
		else {
			current_stack->top -= 1;
			current_matrix;
		}
	}
	catch (int e) {
		std::cerr << "Error: there is only one matrix left in current stack" << std::endl;
	}
}

// Overwrites currentmatrix with m.
void I_my_glLoadMatrixf(const GLfloat *m)
{
	for (int i = 0; i < 16; i++) {
		current_matrix[i] = m[i];
	}
}

void I_my_glLoadMatrixd(const GLdouble *m)
{
	for (int i = 0; i < 16; i++) {
		current_matrix[i] = m[i];
	}
}

void I_my_glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
	GLdouble translationMatrix[16] =
	{ 1, 0, 0, x,
	 0, 1, 0, y,
	 0, 0, 1, z,
	 0, 0, 0, 1 };
	matrix_multiply(translationMatrix);
}

void I_my_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	I_my_glTranslated((GLdouble)x, (GLdouble)y, (GLdouble)z);
}

// Remembers to normalize vector (x, y, z), and to convert angle from degree to radius before calling sin and cos.
void I_my_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	I_my_glPushMatrix();
	normalize(&x, &y, &z);
	GLdouble radians = angle * PI / 180.0;
	GLdouble c = cos(radians);
	GLdouble s = sin(radians);
	GLdouble rotateMatrix[16] =
	{
		(x*x)*(1 - c) + c, (x*y)*(1 - c) - z*s, (x*z)*(1 - c) + y*s, 0,
		(y*x)*(1 - c) + z*s, (y*y)*(1 - c) + c, (y*z)*(1 - c) + x*s, 0,
		(x*z)*(1 - c) + y*s, (y*z)*(1 - c) + x*s, (z*z)*(1 - c) + c, 0,
		0,0,0,1
	};
	matrix_multiply(rotateMatrix);
}

void I_my_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
	I_my_glRotated((GLdouble)angle, (GLdouble)x, (GLdouble)y, (GLdouble)z);
}

void I_my_glScaled(GLdouble x, GLdouble y, GLdouble z)
{
	I_my_glPushMatrix();
	GLdouble scaleMatrix[16] =
	{
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1
	};
	matrix_multiply(scaleMatrix);
}

void I_my_glScalef(GLfloat x, GLfloat y, GLfloat z)
{
	I_my_glScaled((GLdouble)x, (GLdouble)y, (GLdouble)z);
}

// Copies current matrix to m.
void I_my_glGetMatrixf(GLfloat *m)
{
	for (int i = 0; i < 16; i++) {
		m[i] = current_matrix[i];
	}
}

void I_my_glGetMatrixd(GLdouble *m)
{
	for (int i = 0; i < 16; i++) {
		m[i] = current_matrix[i];
	}
}

// Remember to normalize vectors.
void I_my_gluLookAt(GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
	GLdouble centerX, GLdouble centerY, GLdouble centerZ,
	GLdouble upX, GLdouble upY, GLdouble upZ)
{
	// ...
}

void I_my_glFrustum(GLdouble left, GLdouble right, GLdouble bottom,
	GLdouble top, GLdouble zNear, GLdouble zFar)
{
	// ...
}

// Based on the inputs, calculate left, right, bottom, top, and call I_my_glFrustum accordingly
// remember to convert fovy from degree to radius before calling tan.
void I_my_gluPerspective(GLdouble fovy, GLdouble aspect,
	GLdouble zNear, GLdouble zFar)
{
	// ...
}
