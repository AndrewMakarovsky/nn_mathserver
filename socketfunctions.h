#pragma once

#ifndef SOCKETFUNCTIONS_H
#define SOCKETFUNCTIONS_H

#define closeMySocketServer_Function 0
#define nnCreate_Function 1
#define nnLoad_Function 2
#define nnDelete_Function 3
#define nnSetPatterns_Function 4
#define nnSetPatternsFromFiles_Function 5
#define nnLearn_Function 6
#define nnRecognize_Function 7
#define nnSave_Function 8
#define nnGetOutQnt_Function 9
#define nnGetCurrentDirectory_Function 10
#define nnSetCurrentDirectory_Function 11
#define nnReadFile_Function 12
#define nnWriteFile_Function 13
#define nnGeLastError_Function 14
#define nnWriteFileBlock_Function 15

#define nndpCreate_Function 50
#define nndpGetQnt_Function 51
#define nndpGetX_Function 52
#define nndpGetMax_Function 53
#define nndpGetMin_Function 54
#define nndpNormalyze_Function 55
#define nndpDelete_Function 56

#define gauss_Function 100

#define testsocketserver_Function 9999

#pragma pack(push, 1)
//Параметры экспортируемых функций
struct nnCreateParms
{
	int n;
	int nlayers;
	int pqnt;
	int pqnt_blocks;
	int nthreads;
	double rand_epsilon;
	int* nout;
	int tracebufcnt;
	int optimization;
};

struct nnLoadParms
{
	char* fname;
	int tbcnt;         //Размер буфера трассировки
	int optimization;
};

struct nnDeleteParms
{
	long long nnet;
};

struct nnSetPatternsParms
{
	long long nnet;
	int n;
	int m;
};

struct nnSetPatternsFromFilesParms
{
	long long nnet;
	int n;
	int m;
	char* f1;
	char* f2;
};

struct nnLearnParms
{
	long long nnet;
	int maxiter;
	double lambda;
	double epsilon;
	double lambdareg;
};

struct nnRecognizeParms
{
	long long nnet;
	int n;
	int m;
	double* _X;
	double* _Y;
};

struct nnSaveParms
{
	long long nnet;
	char* fname;
};

struct nnWriteFileBlockParms
{
	int filesize;
	int blockqnt;
	int blockno;
	int blocksize;
	int lastblocksize;
};

struct nnGetOutQntParms
{
	long long nnet;
};

struct nnGetCurrentDirectoryParms
{
	long long nnet;
};

struct nnSetCurrentDirectoryParms
{
	long long nnet;
	char* dir;
};

struct nnReadFileParms
{
	long long nnet;
	char* fname;
};

struct nnWriteFileParms
{
	int n;
	char* fname;
};

struct nndpCreateParms
{
	int qnt;
	int n_in;
	int n_out;
};

struct nndpGetQntParms
{
	long long dp;
};

struct nndpGetXParms
{
	long long dp;
};

struct nndpGetMaxParms
{
	long long dp;
};

struct nndpGetMinParms
{
	long long dp;
};

struct nndpNormalyzeParms
{
	long long dp;
};

struct nndpDeleteParms
{
	long long dp;
};

struct gaussParms
{
	int n;
};

struct testsocketserverParms
{
	int qnt;
};
#pragma pack(pop)

#endif
