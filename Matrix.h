#ifndef _MATRIX_H_
#define _MATRIX_H_

#define DEG2RAD(x)	(x = x * M_PI / 180)
#define RAD2DEG(x)	(x = x * 180 / M_PI)
#define DEGREES(x)	((x) * 180 / M_PI)
#define RADIANS(x)	((x) * M_PI / 180)

class Matrix2D
{

public:
		float		matrix[3][3];

					Matrix2D();
					Matrix2D(	float a, float b, float c,
								float d, float e, float f,
								float g = 0, float h = 0, float i = 1);

		void		PrintToStream() const;
		bool		IsIdentity();
		void		Reset();
		
		Matrix2D	&operator=(const Matrix2D &from);
		Matrix2D	&operator=(const float from[3][3]);
		void		SetTo(	float a, float b, float c,
							float d, float e, float f,
							float g = 0, float h = 0, float i = 1);

		Matrix2D	operator*(const Matrix2D &with);
		BPoint		operator*(const BPoint &with);
		Matrix2D	&operator*=(const Matrix2D &with);

		void		TranslateBy(const BPoint &by);
		Matrix2D	&TranslateBySelf(const BPoint &by);
		Matrix2D	TranslateByCopy(const BPoint &by);

		void		ScaleBy(const BPoint &by);
		Matrix2D	&ScaleBySelf(const BPoint &by);
		Matrix2D	ScaleByCopy(const BPoint &by);

		void		RotateBy(float by);
		Matrix2D	&RotateBySelf(float by);
		Matrix2D	RotateByCopy(float by);

		void		SkewBy(const BPoint &by);
		Matrix2D	&SkewBySelf(const BPoint &by);
		Matrix2D	SkewByCopy(const BPoint &by);

		void		Invert();
		Matrix2D	&InvertToSelf();
		Matrix2D	InvertToCopy();

		BRect		ApplyToRect(const BRect &rect);
};

#endif
