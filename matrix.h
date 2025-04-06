//Библиотека динамической компоновки для математических вычислений
//Автор: А.Б. Макаровский
//Версия 1.0
//17 декабря 2023 г.

#pragma once

#include "math.h"
#include <codecvt>
#include <locale>
#include "assert.h"
#include <complex>

using dpoint = double *;
using namespace std;

template <class Tp> class Matrix
{
private:
	Tp* A;

public:
	int n, m; //Размерность n x m
	
	Matrix(int _n, int _m);
	Matrix(int _n, int _m, Tp* a);
	~Matrix();

	Tp* GetPointer();
	Matrix* copy();

	Tp det();

	Matrix& select(int i1, int i2, int j1, int j2);

	Matrix& InsertRow(int i1, Tp* a);

	Matrix& InsertColumn(int j1, Tp* a);
};

template <class Tp> Matrix<Tp>& operator+(Matrix<Tp>& M, Matrix<Tp>& Add);
template <class Tp> Matrix<Tp>& operator-(Matrix<Tp>& M, Matrix<Tp>& Sub);
template <class Tp> void operator+=(Matrix<Tp>& M, Matrix<Tp>& Add);
template <class Tp> void operator-=(Matrix<Tp>& M, Matrix<Tp>& Sub);
template <class Tp> Matrix<Tp>& operator*(Matrix<Tp>& M, Matrix<Tp>& Mult);
template <class Tp> Matrix<Tp>& operator*(Matrix<Tp>& M, Tp s);
template <class Tp> Matrix<Tp>& operator/(Matrix<Tp>& M, Tp s);
template <class Tp> void operator*=(Matrix<Tp>& M, Tp s);
template <class Tp> void operator/=(Matrix<Tp>& M, Tp s);

//Экспортируемые функции
//_DLLAPI long long _stdcall matCreate(int n, int m, char* tp, void* arr = NULL);

//_DLLAPI int _stdcall matGauss(double* arr, int n, double* solution);

