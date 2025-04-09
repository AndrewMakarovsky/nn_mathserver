//���������� ������������ ���������� ��� �������������� ����������
//�����: �.�. ����������� aka ���� �������� � ����������, https://vk.com/andmk
//������ 1.0
//9.04.2025 �.
//�������� �������� ����������� ������ � ������ "����� �����-����� � ��������� ��������� ��������������� ������ 
//� ������������� ������������", �������������� �� ������ https://habr.com/ru/articles/897928/
//������� � 1 ����������� ������ ��������� ����, ��, ��� ��������� � �������� �������, ������ �� ����������� (����� ����� �������).
//��������� ���� ���� ����� ������������ - ����������� ��������� ��������� ��� ���������� ����������� ������������ �������� ������,
//����������� ������ � ����������� ���������� ��� ����������� Xeon � ��.
//���� ��� ������ �����, ������� ������� ������������ (����� �����, �� 100 ���.) �� ����� ����� ����� 4261 0126 3850 2139,
//�� ����� ��� �������� ������ (� �������� ���������� ����� � ������ ������ ���������� ������) �� ���� ���������� ������
//�� �������� https://vk.com/andmk, ���� ��� �������� �������.

#pragma once

#include "math.h"
#include <codecvt>
#include <locale>
#include "assert.h"
#include <complex>
#include <cstdio>
#include <pthread.h>
#include "socketxml.h"
#include "parsexml.h"
#include "nnobject.h"

#define OPTIMIZATION_CPP 0
#define OPTIMIZATION_ASM64 1
#define OPTIMIZATION_ASM64_XEON 2

#define NORMALYZE_NO 0
#define NORMALYZE_SIMPLE 1
#define NORMALYZE_SIMPLE_LINE 2

class NN;

#pragma pack(push, 1)
struct nnLearnResultParms
{
	int iter;
	int success_qnt;
	double norma;
};

struct pthreadData
{
	NN* netw;
	int pqnt_block_no;
	double lambda;
	double epsilon;
	double lambdareg;
	double nrm;
};
#pragma pack(pop)

extern "C" void movsdd(long qnt, void* dest, void* src);

extern "C" void movsdq(long qnt, void* dest, void* src);

//extern "C" void movsDD(int qnt, void* dest, void* src);

extern "C" void matrix8tran(int n, int m, void* src, void* dest);

extern "C" void fill8arr(long n, void* p); //���������� ������� p �� n 8-������� ��������� ������ �� 1 ��������

extern "C" void v8mult(long qnt, void* v1, void* v2, void* ret);

extern "C" void v8add(long qnt, void* v1, void* v2);

extern "C" void v8gsum(unsigned long long qnt, void* gsum, void* x, void* delta);

extern "C" void forwbs(int n, int m, void* z, void* x, void* theta);

extern "C" void Matrix8Tran(int n, int m, void* src, void* dest);

using dpoint = double *;
using namespace std;

class learn_thread_exception : public std::exception
{
	std::string info;
public:
	learn_thread_exception(std::string e = "������ ��� ���������� �������� ��������� ���� (� �������)") : info(e) {}
	const char* what() const noexcept {
		return info.c_str();
	}
};

class NNDataPrepare
{
public:
	int qnt, n_in, n_out;

	double* X;
	double* cv;
	double gmin, gmax;

	NNDataPrepare(int _qnt, int _n_in, int _n_out, double* x);
	~NNDataPrepare();

	void Normalyze(double* in, double* out);

	void EvalCVmatrix(double* x);
};

class NNLayer : public NNObject
{
public:

	int layerno, n, m;   //����� ���� � ����������� ������� �����
	int pqnt;            //�-�� ��������� ��������
	double rand_epsilon;
	int optimization;
	char* Tracebuffer;     //����� �����������
	int tracebufcnt;       //������ ������ �����������
	int* tracebufw;         //�������� � ����� �����������
	double* Theta;         //� ����������� �������� ��������� ��������
	//double* ThetaM;        //��� ������������ ������� ��������� ��������
	//double* ThetaT;
	//double* ThetaMT;
	dpoint* Line;
	//dpoint* LineT;
	double* A;
	double* Z;
	double* In;
	double* Delta;
	double* DeltaB;
	double* Grad;
	double* Gsum;        //��������������� ���������� ��� ���������� ���������
	NNLayer* prev;
	NNLayer* next;

	NNLayer(int _layerno, int _n, int _m, int _pqnt, double _rand_epsilon,
		NNLayer* _prev, NNLayer* _next, int _tracebufcnt, char* _tracebuffer, int* _tracebufw, int opt);

	~NNLayer();

	void ThetaTran();

	void Forward(double* X, double* Y);

	void ForwardCPP(double* X, double* Y);

	void ForwardASM64(double* X, double* Y);

	void ForwardASM64Xeon(double* X, double* Y);

	void BackDelta(double* Y);     //��� ���������� ����

	void BackDelta();              //��� �� ��������� �����

	void BackGrad();

	void BackGradCPP();

	void BackGradASM64();

	void BackGradASM64Xeon();

	void InitIter();

	void UpdateWeights(double lambda = 1.0, double r = 0);

	double Sigmoid(double x);

	double SigmoidP(double x);

	void TInit();

	double Norma2(double* a, double* b, int qnt);

	double Norma2(double* a, int qnt);

	double Norma(double* a, int qnt);
	
	bool isFirst();

	bool isLast();
};

//��������� ���� ��� ������������ ����������
class NN :public NNObject
{
public:

	int in_n, out_n; //�-�� ������ � ������� ����
	int nlayers;     //�-�� �����
	int pqnt;        //�-�� ��������� ��������
	int pqnt_blocks; //�-�� ������ ��������� ��������
	double rand_epsilon;   //��������� ��� ��������� ������������� ������ �����
	int optimization;
	char* Tracebuffer;     //����� �����������
	int tracebufcnt;       //������ ������ �����������
	int *tracebufw;         //�������� � ����� �����������
	double J;        //������� ���������
	int* block_size; //������ �������� ������
	double* X;       //������ ����������� pqnt x in_n - ����� ���� ��� ������� ���������� �������
	double* Y;       //������ ����������� pqnt x out_n - ������ ���� ��� ������� ���������� �������
	double** LineX;  //������ ���������� �� ������ ������� X
	double** LineY;  //������ ���������� �� ������ ������� Y
	double* XL;
	double* YL;
	NNLayer* Wfirst;
	NNLayer* Wlast;
	int nli, nlo;

	NN(int n, int _nlayers, int _pqnt, int _pqnt_blocks, double _rand_epsilon, int* nout, int _tracebufcnt, char* _tracebuffer, int* _tracebufw, int processorno, int opt);

	~NN();

	//�������� pqnt �������� ��� �������� ����
	void SetPatterns(double* _X, double* _Y);

	//double Propagation(double lambda = 1.0, double epsilon = 0.1, double lambdareg = 0);

	double Learn(int pqnt_block_no, double lambda = 1.0, double epsilon = 0.1, double lambdareg = 0);

	void Recognize(double* _X, double* _Y);

	void UpdateWeights(double h, double r);
};

//�������� ����� ��������� ����. �������� ������� ����� � ���� ������������� ������
class NeuralNetwork : public NNObject
{
public:

	int in_n, out_n; //�-�� ������ � ������� ����
	int nlayers;     //�-�� �����
	int nthreads;    //�-�� �������
	int pqnt;        //�-�� ��������� ��������
	int pqnt_blocks; //�-�� ������ ��������� ��������
	int pqnt_block_no; //����� �������� ����� ��������� �������� ��� ��������� ��������
	double rand_epsilon;   //��������� ��� ��������� ������������� ������ �����
	int optimization;
	char* Tracebuffer;     //����� �����������
	int tracebufcnt;       //������ ������ �����������
	int tracebufw;         //�������� � ����� �����������
	char* currentDirectory;
	double J;        //������� ���������
	double* X;       //������ ����������� pqnt x in_n - ����� ���� ��� ������� ���������� �������
	double* Y;       //������ ����������� pqnt x out_n - ������ ���� ��� ������� ���������� �������
	NNLayer* Wfirst;
	NNLayer* Wlast;
	int nli, nlo;
	NN** parnetw;    //������ ���������� �� ��������� ���� ��� ������������ ����������

	NeuralNetwork(int n, int _nlayers, int _pqnt, int _pqnt_blocks, int _nthreads, double _rand_epsilon, int* nout, int _tracebufcnt, char* _tracebuffer, int opt);

	NeuralNetwork(char* fname, int tbcnt, char* buf, int opt);

	~NeuralNetwork();

	//�������� pqnt �������� ��� �������� ����
	void SetPatterns(double* X, double* Y, int norm_type);

	nnLearnResultParms Learn(int maxiter, double lambda = 1.0, double epsilon = 0.1, double lambdareg = 0);

	void Recognize(double* X, double* Y);

	int Save(char* fname);   //���������� ��������� ��������� ����

	char* GetCurrentDirectory();

	void SetCurrentDirectory(char* dir);

private:
	void InpRand();

	double UpdateWeights(double h, double r);

	void ZeroGrad(); //��������� ��������� �� ���� �����

	void GetParallelGrad();

	void SetParallelTheta();

	double NormaIter();

	void UpdateLayersWeights(double h, double r);

	int GetSumQnt();
};

template <class Tp> class Matrix
{
private:
	Tp* A;

public:
	int n, m; //����������� n x m
	
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

void* LearnParallel(void* dt);

void memmove(char* p1, char* p2, int count);

int strlen(char* s);

//�������������� �������
long long nnCreate(int n, int _nlayers, int _pqnt, int _nthreads, double _rand_epsilon, int* nout, int _tracebufcnt = 0, char* _tracebuffer = NULL);

long long nnLoad(char* fname, int tbcnt, char* buf);

void nnDelete(long long nnet);

void nnSetPatterns(long long nnet, double* _X, double* _Y);

double nnLearn(long long nnet, int maxiter = 100, double lambda = 1.0, double epsilon = 0.1, double lambdareg = 0);

void nnRecognize(long long nnet, double* _X, double* _Y);

int nnSave(long long nnet, wchar_t* fname);

int nnGetOutQnt(long long nnet);

long long nndpCreate(int _qnt, int _n_in, int _n_out, double* x);

int nndpGetQnt(long long dp);

void nndpGetX(long long dp, double* x);

double nndpGetMax(long long dp);

double nndpGetMin(long long dp);

void nndpNormalyze(long long dp, double* in, double* out);

void nndpDelete(long long dp);

void WriteToTraceBuffer(int tracebufcnt, char* tracebuffer, int* tracebufw, char* mess);
