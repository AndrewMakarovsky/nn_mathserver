#include "matrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cfenv>
#include <complex>

template <class Tp>
Matrix<Tp>::Matrix(int _n, int _m)
{
	n = _n;
	m = _m;

	A = new Tp[n * m];
}

template <class Tp>
Matrix<Tp>::Matrix(int _n, int _m, Tp* a)
{
	n = _n;
	m = _m;

	A = new Tp[n * m];

	int max = n * m;
	Tp* p = A;
	Tp* p2 = a;

	for (int i = 0; i < max; i++)
	{
		*p++ = *p2++;
	}
}

template <class Tp>
Matrix<Tp>::~Matrix()
{
	delete A;
}
template <class Tp>
Tp* Matrix<Tp>::GetPointer()
{
	return A;
}

template <class Tp>
Matrix<Tp>* Matrix<Tp>::copy()
{
	Matrix<Tp>* ret = new Matrix<Tp>(this->n, this->m, this->GetPointer());

	return ret;
}

template <class Tp>
Matrix<Tp>& Matrix<Tp>::select(int i1, int i2, int j1, int j2)
{
	if (i1 < 0 || i1 >= this->n || i2 < 0 || i2 >= this->n || j1 < 0 || j1 >= this->m || j2 < 0 || j2 >= this->m)
	{
		throw;
	}

	int max = (i2 - i1 + 1) * this->m;
	int rmax = (i2 - i1 + 1) * (j2 - j1 + 1);
	int rn = (i2 - i1 + 1);
	int rm = (j2 - j1 + 1);
	Tp* arr = new Tp[max];
	Tp* p1 = arr;
	Tp* p2 = this->A + i1 * this->m;
	Tp* r = new Tp[rmax];
	Tp* pr = r;

	for (int i = 0; i < max; i++)
	{
		*p1++ = *p2++;
	}

	p1 = arr + j1;

	for (int i = 0; i < rn; i++)
	{
		for (int j = 0; j < rm; j++)
		{
			*pr++ = *p1++;
		}
		p1 += (this->m - rm + j1);
	}

	Matrix<Tp>* ret = new Matrix<Tp>(rn, rm, r);

	delete r;

	return *ret;
}
template <class Tp>
Matrix<Tp>& Matrix<Tp>::InsertRow(int i1, Tp* a)
{

}
template <class Tp>
Matrix<Tp>& Matrix<Tp>::InsertColumn(int j1, Tp* a)
{

}

template <class Tp>
Matrix<Tp>& operator*(Matrix<Tp>& M, double s)
{
	int n = M.n;
	int m = M.m * n;
	Tp* p = M.GetPointer();
	Matrix<Tp>* ret = M.copy();
    Tp* p2 = ret->GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*p2++ = *p++ * s;
	}

	return *ret;
}

template <class Tp>
void operator*=(Matrix<Tp>& M, Tp s)
{
	int n = M.n;
	int m = M.m * n;
	Tp* p = M.GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*p++ *= s;
	}
}

template <class Tp>
Matrix<Tp>& operator/(Matrix<Tp>& M, Tp s)
{
	int n = M.n;
	int m = M.m * n;
	Tp* p = M.GetPointer();
	Matrix<Tp>* ret = M.copy();
	Tp* p2 = ret->GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*p2++ = *p++ / s;
	}

	return *ret;
}

template <class Tp>
void operator/=(Matrix<Tp>& M, Tp s)
{
	int n = M.n;
	int m = M.m * n;
	Tp* p = M.GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*p++ /= s;
	}
}

template <class Tp>
Matrix<Tp>& operator+(Matrix<Tp>& M, Matrix<Tp>& Add)
{
	if (M.n != Add.n || M.m != Add.m)
	{
		throw;
	}

	int n = M.n;
	int m = M.m * n;
	Tp* p = Add.GetPointer();
	Matrix<Tp>* ret = M.copy();
	Tp* pret = ret->GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*pret++ += *p++;
	}

	return *ret;
}

template <class Tp>
Matrix<Tp>& operator-(Matrix<Tp>& M, Matrix<Tp>& Sub)
{
	if (M.n != Sub.n || M.m != Sub.m)
	{
		throw;
	}

	int n = M.n;
    int m = M.m * n;
	Tp* p = Sub.GetPointer();
	Matrix<Tp>* ret = M.copy();
	Tp* pret = ret->GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*pret++ -= *p++;
	}

	return *ret;
}

template <class Tp>
void operator+=(Matrix<Tp>& M, Matrix<Tp>& Add)
{
	if (M.n != Add.n || M.m != Add.m)
	{
		throw;
	}

	int n = M.n;
	int m = M.m * n;
	Tp* p = Add.GetPointer();
	Tp* pret = M.GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*pret++ += *p++;
	}
}

template <class Tp>
void operator-=(Matrix<Tp>& M, Matrix<Tp>& Sub)
{
	if (M.n != Sub.n || M.m != Sub.m)
	{
		throw;
	}

	int n = M.n;
	int m = M.m * n;
	Tp* p = Sub.GetPointer();
	Tp* pret = M.GetPointer();

	for (register int i = 0; i < m; i++)
	{
		*pret++ -= *p++;
	}
}

template <class Tp>
Matrix<Tp>& operator*(Matrix<Tp>& M, Matrix<Tp>& Mult)
{
	if (M.m != Mult.n)
	{
		throw;
	}

	int n = M.n;
	int m = Mult.m;
	int l = M.m;
	Tp* p = M.GetPointer();
	Tp* p2 = Mult.GetPointer();
	Matrix<Tp>* ret = new Matrix<Tp>(n, m);
	Tp* pret = ret->GetPointer();

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			double s = 0;

			for (int k = 0; k < l; k++)
			{
				s += *(p + i * l + k) * *(p2 + k * l + j);
			}

			*(pret + i * l + j) = s;
		}
	}

	return *ret;
}

template <class Tp>
Tp Matrix<Tp>::det()
{
	Tp ret = 0;

	return ret;
}
