#include "matrix.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cfenv>

#define GAUSS_SUCCESS 0
#define GAUSS_NO_OR_MULTIPLE_SOLUTIONS -1
#define GAUSS_FLOAT_ERROR -2
#define GAUSS_ARGUMENS_ERROR -3

const double EPSILON = 1E-127;

using namespace std;

void LineCopy(double* arr, int n, int l1, int l2)
{
	double x;
	double* s1 = arr + (n + 1) * l1;
	double* s2 = arr + (n + 1) * l2;

	for (int i = 0; i <= n; i++)
	{
		x = *s1;
		*s1++ = *s2;
		*s2++ = x;
	}
}

//–ешение систем линейных уравнений методом главного элемента
//ѕараметры
//arr - матрица системы размерности n x (n + 1)
//solution - вектор размерности n дл€ записи решени€ системы
//¬озвращаемое значение: статус решени€
int gauss(double* arr, int n, double* solution)
{
	double* A = arr;
	double* X = solution;
	double* XLast = solution + (n - 1);
	double s;

	if (n < 1)
	{
		return GAUSS_ARGUMENS_ERROR;
	}

	for (int i = 0; i < n - 1; i++)
	{
		double max = *(A + i * (n + 2));
		max = (max > 0) ? max : -max;
		int maxstr = i;
		for (int j = i + 1; j < n; j++)
		{
			double m = *(A + j * (n + 1) + i);
			m = (m > 0) ? m : -m;
			if (m > max)
			{
				max = m;
				maxstr = j;
			}
		}

		if (max < EPSILON)
		{
			return GAUSS_NO_OR_MULTIPLE_SOLUTIONS;
		}

		if (maxstr != i)
		{
			LineCopy(A, n, i, maxstr);
		}

		double w;
		double* p = (A + i * (n + 2));
		double v = *p;

		for (int j = i + 1; j < n; j++)
		{
			double* p2 = (A + j * (n + 1) + i);
			p = (A + i * (n + 2));

			w = *p2 / v;
			p++;
			*p2++ = 0;

			for (int k = i + 1; k < n + 1; k++)
			{
				*p2 = *p2 - *p * w;
				p++;
				p2++;
			}
		}
	}

	A = arr;
	X = XLast;
	for (int i = n - 1; i >= 0; i--)
	{
		double* p = A + i * (n + 1) + (n - 1);
		s = *(p + 1);
		double* x = XLast;

		for (int j = n - 1; j > i; j--)
		{
			s -= ((*x--) * (*p--));
		}

		double c = *p;

		int raised;
		feclearexcept(FE_ALL_EXCEPT);
		*X-- = s / c;
		raised = fetestexcept(FE_ALL_EXCEPT);
		if (raised)
		{
			return GAUSS_FLOAT_ERROR;
		}
	}

	return GAUSS_SUCCESS;
}
