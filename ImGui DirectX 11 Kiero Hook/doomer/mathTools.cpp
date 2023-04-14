#include "../includes.h"
#define PI 3.14159265358979323846

float rad(float deg) {
	return deg * (PI / 180);
}

float* rotToMat(float x, float y, float z) {
	static float* mat[9];
	x = rad(x);
	y = rad(y);
	z = rad(z + 180); 

	//convert to quaternion
	float sinTh = cos(z / 2);
	float ry = cos(x / 2);
	float sinth3 = cos(y / 2);
	float cosTh = sin(z / 2);
	float rx = sin(x / 2);
	float costh3 = sin(y / 2);

	float q0 = cosTh * ry * sinth3 + sinTh * rx * costh3;
	float q1 = sinTh * rx * sinth3 - cosTh * ry * costh3;
	float q2 = sinTh * ry * costh3 + cosTh * rx * sinth3;
	float q3 = sinTh * ry * sinth3 - cosTh * rx * costh3;

	//convert to mat
	float a1[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	a1[0] = 2 * (q0 * q0 + q1 * q1) - 1;
	a1[1] = 2 * (q1 * q2 - q0 * q3);
	a1[2] = 2 * (q1 * q3 + q0 * q2);
	a1[3] = 2 * (q1 * q2 + q0 * q3);
	a1[4] = 2 * (q0 * q0 + q2 * q2) - 1;
	a1[5] = 2 * (q2 * q3 - q0 * q1);
	a1[6] = 2 * (q1 * q3 - q0 * q2);
	a1[7] = 2 * (q2 * q3 + q0 * q1);
	a1[8] = 2 * (q0 * q0 + q3 * q3) - 1;

	return a1;
}