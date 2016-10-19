

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include "i_my_gl.h" 

// Defines maximum stack capacity.
#define STACK_CAP 16
// Defines pi for converting angles.
#define PI 3.14159265359
#define current(row,col) current[(col<<2)+row]
#define b(row,col) b[(col<<2)+row]

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
	GLdouble temp[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[j * 4 + i] = current_matrix[0 * 4 + i] * b[j * 4 + 0] +
				current_matrix[1 * 4 + i] * b[j * 4 + 1] +
				current_matrix[2 * 4 + i] * b[j * 4 + 2] +
				current_matrix[3 * 4 + i] * b[j * 4 + 3];
		}
	}
	for (int i = 0; i < 16; i++) {
		current_matrix[i] = temp[i];
	}
}

// Calculates cross product of b and c, put the result in a
// a = b x c.
void cross_product(GLdouble *ax, GLdouble *ay, GLdouble *az,
	GLdouble bx, GLdouble by, GLdouble bz,
	GLdouble cx, GLdouble cy, GLdouble cz)
{
	*ax = by*cz - cy*bz;
	*ay = bz*cx - cz*bx;
	*az = bx*cy - cx*by;
}

// Normalizes vector (x, y, z).
void normalize(GLdouble *x, GLdouble *y, GLdouble *z)
{
	GLdouble length = (GLdouble)sqrt(*x * *x + *y * *y + *z * * z);
	*x = *x / length;
	*y = *y / length;
	*z = *z / length;
}

// Switches matrix mode by changing the current stack pointer.
void I_my_glMatrixMode(GLenum mode)
{
	switch (mode) {
	case GL_MODELVIEW:
		current_stack = &model_view;
		break;
	case GL_PROJECTION:
		current_stack = &projection;
		break;
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
	if (current_stack->top < STACK_CAP - 1) {
		for (int i = 0; i < 16; i++) {
			current_stack->m[current_stack->top + 1][i] = current_matrix[i];
		}
	}
	else {
		std::cout << "Error: current stack is full" << std::endl;
	}
}

// Pops current matrix from current stack, report error if the stack has only one matrix left.
void I_my_glPopMatrix(void)
{
	if (current_stack->top > 1) {
		current_stack->top--;
	}
	else {
		std::cout << "Error: there is only one matrix left in current stack" << std::endl;
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
	{ 1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1, 0,
	  x, y, z, 1
	};
	matrix_multiply(translationMatrix);
}

void I_my_glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
	I_my_glTranslated((GLdouble)x, (GLdouble)y, (GLdouble)z);
}

// Remembers to normalize vector (x, y, z), and to convert angle from degree to radius before calling sin and cos.
void I_my_glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
	normalize(&x, &y, &z);
	GLdouble radians = angle *PI / 180.0;
	GLdouble s = sin(radians);
	GLdouble c = cos(radians);
	GLdouble xx = x*x;
	GLdouble xy = x*y;
	GLdouble xz = x*z;
	GLdouble yy = y*y;
	GLdouble yz = y*z;
	GLdouble zz = z*z;

	GLdouble rotationMatrix[16];
	rotationMatrix[0] = xx + (1 - xx) * c;
	rotationMatrix[4] = xy*(1 - c) - z*s;
	rotationMatrix[8] = xz*(1 - c) + y*s;
	rotationMatrix[12] = 0;

	rotationMatrix[1] = xy*(1 - c) + z*s;
	rotationMatrix[5] = yy + (1 - yy)*c;
	rotationMatrix[9] = yz*(1 - c) - x*s;
	rotationMatrix[13] = 0;

	rotationMatrix[2] = xz*(1 - c) - y*s;
	rotationMatrix[6] = yz*(1 - c) + x*s;
	rotationMatrix[10] = zz + (1 - zz)*c;
	rotationMatrix[14] = 0;

	rotationMatrix[3] = 0;
	rotationMatrix[7] = 0;
	rotationMatrix[11] = 0;
	rotationMatrix[15] = 1;
	matrix_multiply(rotationMatrix);
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
		m[i] = (GLfloat)current_matrix[i];
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
	GLdouble sx, sy, sz;
	GLdouble fx = centerX - eyeX,
		fy = centerY - eyeY,
		fz = centerZ - eyeZ;
	normalize(&fx, &fy, &fz);
	cross_product(&sx, &sy, &sz, fx, fy, fz, upX, upY, upZ);
	normalize(&sx, &sy, &sz);
	cross_product(&upX, &upY, &upZ, sx, sy, sz, fx, fy, fz);

	GLdouble lookAtMatrix[16];
	lookAtMatrix[0] = sx;
	lookAtMatrix[8] = sy;
	lookAtMatrix[4] = sz;
	lookAtMatrix[12] = 0;

	lookAtMatrix[1] = upX;
	lookAtMatrix[5] = upY;
	lookAtMatrix[9] = upZ;
	lookAtMatrix[13] = 0;

	lookAtMatrix[2] = -fx;
	lookAtMatrix[6] = -fy;
	lookAtMatrix[10] = -fz;
	lookAtMatrix[14] = 0;

	lookAtMatrix[3] = 0;
	lookAtMatrix[7] = 0;
	lookAtMatrix[11] = 0;
	lookAtMatrix[15] = 1;

	matrix_multiply(lookAtMatrix);
}

void I_my_glFrustum(GLdouble left, GLdouble right, GLdouble bottom,
	GLdouble top, GLdouble zNear, GLdouble zFar)
{
	if (zNear <= 0 || zFar <= 0 || left == right || bottom == top || zNear == zFar) {
		std::cout << "Error: Invalid Inputs" << std::endl;
	}

	GLdouble	A = (right + left) / (right - left),
				B = (top + bottom) / (top - bottom),
				C = -((zFar + zNear) / (zFar - zNear)),
				D = -2 * ((zFar + zNear) / (zFar - zNear));

	GLdouble frustumMatrix[16] = {
		(2 * zNear) / (right - left), 0, 0, 0,
		0, (2 * zNear) / (top - bottom), 0, 0,
		A, B, C, -1,
		0,0,0,D
	};
	matrix_multiply(frustumMatrix);
}

// Based on the inputs, calculate left, right, bottom, top, and call I_my_glFrustum accordingly
// remember to convert fovy from degree to radius before calling tan.
void I_my_gluPerspective(GLdouble fovy, GLdouble aspect,
	GLdouble zNear, GLdouble zFar)
{
	GLdouble top = tan(((fovy*PI) / 360)) * zNear;
	GLdouble bottom = -top;
	GLdouble right = top * aspect;
	GLdouble left = bottom * aspect;
	I_my_glFrustum(left, right, bottom, top, zNear, zFar);
}
