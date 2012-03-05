#include <math.h>
#include <stdio.h>
#include <string.h>
#include <Point.h>
#include <Rect.h>
#include <sys/param.h>
#include "Matrix.h"

Matrix2D::Matrix2D()
{
	memset(matrix, 0, sizeof(matrix));
	matrix[0][0] = 1; matrix[1][1] = 1; matrix[2][2] = 1;
}

Matrix2D::Matrix2D(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
	SetTo(a, b, c, d, e, f, g, h, i);
}

Matrix2D &
Matrix2D::operator=(const Matrix2D &from)
{
	memcpy(matrix, from.matrix, sizeof(from.matrix));
	return *this;
}

Matrix2D &
Matrix2D::operator=(const float from[3][3])
{
	memcpy(matrix, from, sizeof(from) * 9);
	return *this;
}

void
Matrix2D::SetTo(float a, float b, float c, float d, float e, float f, float g, float h, float i)
{
	matrix[0][0] = a; matrix[0][1] = b; matrix[0][2] = c;
	matrix[1][0] = d; matrix[1][1] = e; matrix[1][2] = f;
	matrix[2][0] = g; matrix[2][1] = h; matrix[2][2] = i;
}

Matrix2D
Matrix2D::operator*(const Matrix2D &with)
{
	Matrix2D res;
	
	for (int j = 0; j < 3; j++)
		for (int i = 0; i < 3; i++)
			res.matrix[i][j] =    matrix[i][0] * with.matrix[0][j]
								+ matrix[i][1] * with.matrix[1][j]
								+ matrix[i][2] * with.matrix[2][j];
	
	return res;
}

BPoint
Matrix2D::operator*(const BPoint &with)
{
	BPoint res;
	
	res.x = with.x * matrix[0][0] + with.y * matrix[0][1] + matrix[0][2];
	res.y = with.x * matrix[1][0] + with.y * matrix[1][1] + matrix[1][2];
	
//	[	x	y	1	] *	[	a	b	c	]
//						[	d	e	f	]	=	[	x	y	]
	
	return res;
}

Matrix2D &
Matrix2D::operator*=(const Matrix2D &with)
{
	*this = *this * with;
	return *this;
}

void
Matrix2D::TranslateBy(const BPoint &by)
{
	Matrix2D temp(1, 0, by.x, 0, 1, by.y);
	*this *= temp;
}

Matrix2D &
Matrix2D::TranslateBySelf(const BPoint &by)
{
	this->TranslateBySelf(by);
	return *this;
}

Matrix2D
Matrix2D::TranslateByCopy(const BPoint &by)
{
	Matrix2D *res = new Matrix2D(*this);
	res->TranslateBy(by);
	return *res;
}

void
Matrix2D::ScaleBy(const BPoint &by)
{
	Matrix2D temp(by.x, 0, 0, 0, by.y, 0);
	*this *= temp;
}

Matrix2D &
Matrix2D::ScaleBySelf(const BPoint &by)
{
	this->ScaleBy(by);
	return *this;
}

Matrix2D
Matrix2D::ScaleByCopy(const BPoint &by)
{
	Matrix2D *res = new Matrix2D(*this);
	res->ScaleBy(by);
	return *res;
}

void
Matrix2D::RotateBy(float by)
{
	DEG2RAD(by);
	Matrix2D temp(cos(by), -sin(by), 0, sin(by), cos(by), 0);
	*this *= temp;
}

Matrix2D &
Matrix2D::RotateBySelf(float by)
{
	this->RotateBy(by);
	return *this;
}

Matrix2D
Matrix2D::RotateByCopy(float by)
{
	Matrix2D *res = new Matrix2D(*this);
	res->RotateBy(by);
	return *res;
}

void
Matrix2D::SkewBy(const BPoint &by)
{
	float x = by.x;
	float y = by.y;
	DEG2RAD(x);
	DEG2RAD(y);
	Matrix2D temp(1, tan(x), 0, tan(y), 1, 0);
	*this *= temp;
}

Matrix2D &
Matrix2D::SkewBySelf(const BPoint &by)
{
	this->SkewBy(by);
	return *this;
}

Matrix2D
Matrix2D::SkewByCopy(const BPoint &by)
{
	Matrix2D *res = new Matrix2D(*this);
	res->SkewBy(by);
	return *res;
}

void
Matrix2D::PrintToStream() const
{
	printf("Matrix2D(a:%f, b:%f, c:%f \n", matrix[0][0], matrix[0][1], matrix[0][2]);
	printf("         d:%f, e:%f, f:%f \n", matrix[1][0], matrix[1][1], matrix[1][2]);
	printf("         g:%f, h:%f, i:%f)\n", matrix[2][0], matrix[2][1], matrix[2][2]);
}

bool
Matrix2D::IsIdentity()
{
	return (matrix[0][0] == 1) && (matrix[0][1] == 0) && (matrix[0][2] == 0)
		&& (matrix[1][0] == 0) && (matrix[1][1] == 1) && (matrix[1][2] == 0)
		&& (matrix[2][0] == 0) && (matrix[2][1] == 0) && (matrix[2][2] == 1);
}

void
Matrix2D::Reset()
{
	float identity[9] = { 1, 0, 0, 0, 1, 0, 0, 0, 1 };
	memcpy(matrix, identity, sizeof(float) * 9);
}

void
Matrix2D::Invert()
{
	float temp[3][3];
	memcpy(temp, matrix, sizeof(float) * 9);
	
	matrix[0][1] = temp[1][0];
	matrix[0][2] = temp[2][0];
	matrix[1][0] = temp[0][1];
	matrix[1][2] = temp[2][1];
	matrix[2][0] = temp[0][2];
	matrix[2][1] = temp[1][2];
}

Matrix2D &
Matrix2D::InvertToSelf()
{
	this->Invert();	
	return *this;
}

Matrix2D
Matrix2D::InvertToCopy()
{
	Matrix2D *res = new Matrix2D(*this);
	res->Invert();
	return *res;
}

BRect
Matrix2D::ApplyToRect(const BRect &rect)
{
	BRect result;
	BPoint points[4] = { rect.LeftTop(), rect.LeftBottom(), rect.RightTop(), rect.RightBottom() };
	points[0] = *this * points[0];
	points[1] = *this * points[1];
	points[2] = *this * points[2];
	points[3] = *this * points[3];
	
	result.left = MIN(points[0].x, MIN(points[1].x, MIN(points[2].x, points[3].x)));
	result.right = MAX(points[0].x, MAX(points[1].x, MAX(points[2].x, points[3].x)));
	result.top = MIN(points[0].y, MIN(points[1].y, MIN(points[2].y, points[3].y)));
	result.bottom = MAX(points[0].y, MAX(points[1].y, MAX(points[2].y, points[3].y)));
	return result;
}
