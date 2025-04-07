// neuralnetw.cpp : Defines the exported functions for the DLL.
#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <limits.h>
#include "neuralnetw.h"
#include <unistd.h>

//using namespace concurrency;
using namespace std;
using namespace socketxml;

NeuralNetwork::NeuralNetwork(int n, int _nlayers, int _pqnt, int _pqnt_blocks, int _nthreads, double _rand_epsilon, int* nout, int _tracebufcnt, char* _tracebuffer, int opt) : NNObject(string("NeuralNetwork"))
{
	in_n = n;
	nlayers = _nlayers;
	pqnt = _pqnt;
	pqnt_blocks = _pqnt_blocks;
	nthreads = _nthreads;
	rand_epsilon = _rand_epsilon;
	Tracebuffer = _tracebuffer;
	tracebufcnt = _tracebufcnt;
	tracebufw = 0;
	optimization = opt;

	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		currentDirectory = new char[PATH_MAX];
		copystr(currentDirectory, cwd, PATH_MAX);
	}
	else
	{
		currentDirectory = NULL;
	}

	J = 0;
	X = NULL;
	Y = NULL;
	Wfirst = NULL;
	nli = nlo = 0;
	parnetw = (NN **)new char[sizeof(NN *) * nthreads];

	if (tracebufcnt > 0)
	{
		char* p = Tracebuffer;

		for (int i = 0; i < tracebufcnt; i++)
		{
			*p++ = (char)0;
		}
	}

	int n1 = n;
	NNLayer* prev = NULL;
	for (int i = 1; i < nlayers; i++)
	{
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, (char*)"Создание слоя\n");
		NNLayer* p = new NNLayer(i + 1, nout[i - 1], n1, pqnt, rand_epsilon, prev, NULL, tracebufcnt, Tracebuffer, &tracebufw, opt);
		if (prev != NULL)
		{
			prev->next = p;
			if (n1 > nli)
			{
				nli = n1;
			}
			if (nout[i - 1] > nlo)
			{
				nlo = nout[i - 1];
			}
		}
		else
		{
			Wfirst = p;
			nli = n1;
			nlo = nout[i - 1];
		}

		prev = p;
		n1 = nout[i - 1];
	}

	Wlast = prev;
	out_n = n1;

	X = new double[pqnt * in_n];
	Y = new double[pqnt * out_n];

	WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, (char*)"Создание нейронных сетей для параллельных вычислений\n");
	//Создание нейронных сетей для параллельных вычислений
	int qnt = pqnt / nthreads;
	for (int i = 0; i < nthreads; i++)
	{
		if (i == (nthreads - 1))
		{
			qnt = pqnt - i * qnt;
		}
		if (qnt > 0)
		{
			*(parnetw + i) = new NN(n, _nlayers, qnt, pqnt_blocks, _rand_epsilon, nout, tracebufcnt, Tracebuffer, &tracebufw, i + 1, opt);
		}
	}

	WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, (char*)"Копирование матрицы весов\n");
	//Копирование матрицы весов
	for (int i = 0; i < nthreads; i++)
	{
		NNLayer* cld = parnetw[i]->Wfirst;
		NNLayer* cl = Wfirst;
		while (cl)
		{
			int max = cl->n * (cl->m + 1);
			double* p1 = cld->Theta;
			double* p2 = cl->Theta;
			double* p3 = cld->ThetaT;
			double* p4 = cl->ThetaT;
			for (int j = 0; j < max; j++)
			{
				*p1++ = *p2++;
				*p3++ = *p4++;
			}

			cld = cld->next;
			cl = cl->next;
		}
	}

	/*for (int i = 0; i < nthreads; i++)
	{
		NNLayer* cld = parnetw[i]->Wfirst;
		NNLayer* cl = Wfirst;
		while (cl)
		{
			int max = cl->n * cl->m;
			double* p1 = cld->ThetaM;
			double* p2 = cl->ThetaM;
			double* p3 = cld->ThetaMT;
			double* p4 = cl->ThetaMT;
			for (int j = 0; j < max; j++)
			{
				*p1++ = *p2++;
				*p3++ = *p4++;
			}

			cld = cld->next;
			cl = cl->next;
		}
	}*/

	if (tracebufcnt - tracebufw > 0)
	{
		const char* mess = "Neural network is created\n";
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, (char*)mess);
	}
}

NeuralNetwork::NeuralNetwork(char* fname, int tbcnt, char* buf, int opt) : NNObject(string("NeuralNetwork"))
{
	optimization = OPTIMIZATION_CPP;

	std::ifstream fin(fname, std::ifstream::binary | std::ifstream::in);
	if (fin.is_open())
	{
		fin.read((char*)&in_n, sizeof(int));
		fin.read((char*)&out_n, sizeof(int));
		fin.read((char*)&nlayers, sizeof(int));
		fin.read((char*)&nthreads, sizeof(int));
		fin.read((char*)&pqnt, sizeof(int));
		fin.read((char*)&pqnt_blocks, sizeof(int));
		//fin.read((char*)&tracebufcnt, sizeof(int));

		//fin >> in_n >> out_n >> nlayers >> nthreads >> pqnt;
		//fin >> tracebufcnt;

		int* p_nout = new int[nlayers - 1];

		J = 0;
		X = NULL;
		Y = NULL;
		Wfirst = NULL;
		nli = nlo = 0;
		parnetw = (NN**)new char[sizeof(NN*) * nthreads];

		tracebufcnt = tbcnt;
		Tracebuffer = buf;
		tracebufw = 0;

		NNLayer* nl = NULL;
		NNLayer* nl_prev = NULL;
		NNLayer* prev = NULL;
		for (int j = 1; j < nlayers; j++)
		{
			int l_no, l_in_n, l_out_n, l_pqnt;

			//fin >> l_no >> l_in_n >> l_out_n >> l_pqnt;

			fin.read((char*)&l_no, sizeof(int));
			fin.read((char*)&l_in_n, sizeof(int));
			fin.read((char*)&l_out_n, sizeof(int));
			fin.read((char*)&l_pqnt, sizeof(int));

			nl_prev = nl;
			nl = new NNLayer(l_no, l_out_n, l_in_n, l_pqnt, 0, prev, NULL, tracebufcnt, Tracebuffer, &tracebufw, opt);
			prev = nl;
			p_nout[j - 1] = l_out_n;

			if (j == 1)
			{
				Wfirst = nl;
			}
			else
			{
				nl_prev->next = nl;
			}

			if (j == nlayers - 1)
			{
				Wlast = nl;
			}

			fin.read((char*)nl->Theta, l_out_n * (l_in_n + 1) * sizeof(double));
			//fin.read((char*)nl->ThetaM, l_out_n * l_in_n * sizeof(double));
			fin.read((char*)nl->ThetaT, l_out_n * (l_in_n + 1) * sizeof(double));
			//fin.read((char*)nl->ThetaMT, l_out_n * l_in_n * sizeof(double));
		}

		for (int i = 0; i < nthreads; i++)
		{
			int t_in, t_out, t_layers, t_pqnt, t_pqnt_blocks;

			//fin >> t_in >> t_out >> t_layers >> t_pqnt;

			fin.read((char*)&t_in, sizeof(int));
			fin.read((char*)&t_out, sizeof(int));
			fin.read((char*)&t_layers, sizeof(int));
			fin.read((char*)&t_pqnt, sizeof(int));
			fin.read((char*)&t_pqnt_blocks, sizeof(int));

			parnetw[i] = new NN(in_n, nlayers, t_pqnt, t_pqnt_blocks, 0, p_nout, tracebufcnt, Tracebuffer, &tracebufw, i + 1, opt);

			nl = NULL;
			nl_prev = NULL;
			prev = NULL;
			for (int j = 1; j < nlayers; j++)
			{
				int l_no, l_in_n, l_out_n, l_pqnt;

				//fin >> l_no >> l_in_n >> l_out_n >> l_pqnt;

				fin.read((char*)&l_no, sizeof(int));
				fin.read((char*)&l_in_n, sizeof(int));
				fin.read((char*)&l_out_n, sizeof(int));
				fin.read((char*)&l_pqnt, sizeof(int));

				nl_prev = nl;
				nl = new NNLayer(l_no, l_out_n, l_in_n, l_pqnt, 0, prev, NULL, tracebufcnt, Tracebuffer, &tracebufw, opt);
				prev = nl;
				p_nout[j - 1] = l_out_n;

				if (j == 1)
				{
					parnetw[i]->Wfirst = nl;
				}
				else
				{
					nl_prev->next = nl;
				}

				if (j == nlayers - 1)
				{
					parnetw[i]->Wlast = nl;
				}

				fin.read((char*)nl->Theta, l_out_n * (l_in_n + 1) * sizeof(double));
				//fin.read((char*)nl->ThetaM, l_out_n * l_in_n * sizeof(double));
				fin.read((char*)nl->ThetaT, l_out_n * (l_in_n + 1) * sizeof(double));
				//fin.read((char*)nl->ThetaMT, l_out_n * l_in_n * sizeof(double));
			}
		}

		X = new double[pqnt * in_n];
		Y = new double[pqnt * out_n];

		/*//Копирование матрицы весов
		for (int i = 0; i < nthreads; i++)
		{
			NNLayer* cld = parnetw[i]->Wfirst;
			NNLayer* cl = Wfirst;
			while (cl)
			{
				int max = cl->n * (cl->m + 1);
				double* p1 = cld->Theta;
				double* p2 = cl->Theta;
				double* p3 = cld->ThetaT;
				double* p4 = cl->ThetaT;
				for (int j = 0; j < max; j++)
				{
					*p1++ = *p2++;
					*p3++ = *p4++;
				}

				cld = cld->next;
				cl = cl->next;
			}
		}*/

		if (tracebufcnt - tracebufw > 0)
		{
			const char* mess = "Neural network is created\n";
			WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, (char*)mess);
		}
	}
}

NeuralNetwork::~NeuralNetwork()
{
	NNLayer* p = Wlast;
	NNLayer* prev = NULL;
	char mess[100];

	for (int i = 0; i < 100; i++)
	{
		mess[i] = 0;
	}

	try
	{
		delete[] X;
		delete[] Y;
		delete[] currentDirectory;
	}
	catch(const std::exception& e)
	{
		sprintf(mess, "Деструктор NeuralNetwork точка 1: %s\n", e.what());
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, mess);
	}

	try
	{
		for (int i = 1; i < nlayers; i++)
		{
			prev = p->prev;
			delete p;

			if (prev == NULL)
			{
				break;
			}
			else
			{
				p = prev;
			}
		}
	}
	catch (const std::exception& e)
	{
		sprintf(mess, "Деструктор NeuralNetwork точка 2: %s\n", e.what());
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, mess);
	}

	try
	{
		for (int i = 0; i < nthreads; i++)
		{
			delete parnetw[i];
		}
	}
	catch (const std::exception& e)
	{
		sprintf(mess, "Деструктор NeuralNetwork точка 3: %s\n", e.what());
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, mess);
	}

	try
	{
		delete[] parnetw;
	}
	catch (const std::exception& e)
	{
		sprintf(mess, "Деструктор NeuralNetwork точка 4: %s\n", e.what());
		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, mess);
	}
}

//Загрузка pqnt образцов для обучения сети
void NeuralNetwork::SetPatterns(double* _X, double* _Y)
{
	double* x = X;
	double* y = Y;
	double* _x = _X;
	double* _y = _Y;

	for (int i = 0; i < pqnt * in_n; i++)
	{
		*x++ = *_x++;
	}

	for (int i = 0; i < pqnt * out_n; i++)
	{
		*y++ = *_y++;
	}

	InpRand();

	x = X;
	y = Y;

	for (int i = 0; i < nthreads; i++)
	{
		parnetw[i]->SetPatterns(x, y);

		x = x + parnetw[i]->pqnt * in_n;
		y = y + parnetw[i]->pqnt * out_n;
	}
}

void NeuralNetwork::InpRand()
{
	double d;
	int r;
	double* x0 = new double[in_n];
	double* y0 = new double[out_n];

	for (int i = pqnt - 1; i > 0; i--)
	{
		d = ((double)rand()) / RAND_MAX;
		r = (int)(d * (i + 1));
		r = (r == i + 1) ? i : r;

		//Сохранение строки i
		double* p1 = x0;
		double* p2 = X + i * in_n;
		for (int j = 0; j < in_n; j++)
		{
			*p1++ = *p2++;
		}

		p1 = y0;
		p2 = Y + i * out_n;
		for (int j = 0; j < out_n; j++)
		{
			*p1++ = *p2++;
		}

		//Копирование строки r в строку i
		p1 = X + i * in_n;
		p2 = X + r * in_n;
		for (int j = 0; j < in_n; j++)
		{
			*p1++ = *p2++;
		}

		p1 = Y + i * out_n;
		p2 = Y + r * out_n;
		for (int j = 0; j < out_n; j++)
		{
			*p1++ = *p2++;
		}

		//Копирование сохраненной строки i в строку r
		p2 = x0;
		p1 = X + r * in_n;
		for (int j = 0; j < in_n; j++)
		{
			*p1++ = *p2++;
		}

		p2 = y0;
		p1 = Y + r * out_n;
		for (int j = 0; j < out_n; j++)
		{
			*p1++ = *p2++;
		}
	}

	delete y0;
	delete x0;
}

nnLearnResultParms NeuralNetwork::Learn(int maxiter, double lambda, double epsilon, double lambdareg)
{
	double norma;
	int iter;
	double* px;
	double* py;
	double* py0;
	int success_qnt = 0;
	NN** netw = parnetw;
	pqnt_block_no = 0;

	for (iter = 0; iter < maxiter; iter++)
	{
		norma = 0;

		pthreadData* threadData = new pthreadData[nthreads];
		pthread_t* mythread = new pthread_t[nthreads];

		for (int i = 0; i < nthreads; i++)
		{
			threadData[i].netw = parnetw[i];
			threadData[i].pqnt_block_no = pqnt_block_no;
			threadData[i].lambda = lambda;
			threadData[i].epsilon = epsilon;
			threadData[i].lambdareg = lambdareg;
	        threadData[i].nrm = 0;

		    pthread_create(&mythread[i], NULL, LearnParallel, &threadData[i]);
		}

		for (int i = 0; i < nthreads; i++)
		{
			pthread_join(mythread[i], NULL);
		}

		for (int i = 0; i < nthreads; i++)
		{
			if (threadData[i].nrm < 0)
			{
				throw learn_thread_exception();
			}

			norma += threadData[i].nrm;
		}

		norma /= GetSumQnt();

		delete[] threadData;
		delete[] mythread;

		UpdateWeights(lambda, lambdareg);

		if (tracebufcnt - tracebufw > 0)
		{
			char mess[80];

			sprintf(mess, "Iteration %d: J = %lf, norma = %lf\n", iter, J, norma);
			WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, mess);
		}

		pqnt_block_no++;
		if (pqnt_block_no >= pqnt_blocks)
		{
			pqnt_block_no = 0;
		}

		/*if (norma < epsilon)
		{
			break;
		}*/
	}

	px = X;
	py = Y;
	py0 = new double[out_n];
	double max;
	int et, res;

	for (int i = 0; i < pqnt; i++)
	{
		Recognize(px, py0);
		max = 0;
		et = 0;
		for (int j = 0; j < out_n; j++)
		{
			if (max < *(py + j))
			{
				max = *(py + j);
				et = j;
			}
		}
		max = 0;
		res = 0;
		for (int j = 0; j < out_n; j++)
		{
			if (max < *(py0 + j))
			{
				max = *(py0 + j);
				res = j;
			}
		}
		if (et == res)
		{
			success_qnt++;
		}

		px += in_n;
		py += out_n;
	}

	delete[] py0;

	nnLearnResultParms lrp;
	lrp.iter = iter;
	lrp.success_qnt = success_qnt;
	lrp.norma = norma;

	return lrp;
}

void* LearnParallel(void* dt)
{
	pthreadData* threadData = (pthreadData*) dt;

	try
	{
		threadData->nrm += threadData->netw->Learn(threadData->pqnt_block_no, threadData->lambda, threadData->epsilon, threadData->lambdareg);
	}
	catch (exception&)
	{
		threadData->nrm = -1;
	}
}

double NeuralNetwork::UpdateWeights(double h, double r)
{
	double norma = 0;
	
	ZeroGrad();

	GetParallelGrad();

	UpdateLayersWeights(h, r);

	SetParallelTheta();

	return norma;
}

void NeuralNetwork::UpdateLayersWeights(double h, double r)
{
	NNLayer* tcl = Wfirst;

	while (tcl)
	{
		tcl->UpdateWeights(h, r);

		tcl = tcl->next;
	}
}

double NeuralNetwork::NormaIter()
{
	NNLayer* tcl = Wlast;
	double norma = 0;

	double* p1 = tcl->Delta;
	int max = tcl->n;
	for (int j = 0; j < max; j++)
	{
		norma += (*p1 * *p1);
		p1++;
	}

	return norma;
}

void NeuralNetwork::ZeroGrad()
{
	NNLayer* tcl = Wfirst;
	while (tcl)
	{
		double* p1 = tcl->Grad;
		int max = tcl->n * (tcl->m + 1);
		for (int j = 0; j < max; j++)
		{
			*p1++ = 0;
		}

		tcl = tcl->next;
	}
}

void  NeuralNetwork::GetParallelGrad()
{
	int sumqnt = GetSumQnt();

	J = 0;
	for (int i = 0; i < nthreads; i++)
	{
		NN* nn = parnetw[i];
		J += nn->J;
		NNLayer* cl = nn->Wfirst;
		NNLayer* tcl = Wfirst;
		while (cl)
		{
			double* p1 = tcl->Grad;
			double* p2 = cl->Grad;
			int max = tcl->n * (tcl->m + 1);
			for (int j = 0; j < max; j++)
			{
				*p1++ += (*p2 / sumqnt);
				p2++;
			}

			tcl = tcl->next;
			cl = cl->next;
		}
	}

	J /= sumqnt;
}

void  NeuralNetwork::SetParallelTheta()
{
	for (int i = 0; i < nthreads; i++)
	{
		NN* nn = parnetw[i];
		NNLayer* cl = nn->Wfirst;
		NNLayer* tcl = Wfirst;
		while (cl)
		{
			double* p1 = tcl->Theta;
			double* p2 = cl->Theta;
			double* p3 = tcl->ThetaT;
			double* p4 = cl->ThetaT;

			int max = tcl->n * (tcl->m + 1);
			for (int j = 0; j < max; j++)
			{
				*p2++ = *p1++;
				*p4++ = *p3++;
			}

			tcl = tcl->next;
			cl = cl->next;
		}
	}

	/*for (int i = 0; i < nthreads; i++)
	{
		NN* nn = parnetw[i];
		NNLayer* cl = nn->Wfirst;
		NNLayer* tcl = Wfirst;
		while (cl)
		{
			double* p1 = tcl->ThetaM;
			double* p2 = cl->ThetaM;
			double* p3 = tcl->ThetaMT;
			double* p4 = cl->ThetaMT;

			int max = tcl->n * tcl->m;
			for (int j = 0; j < max; j++)
			{
				*p2++ = *p1++;
				*p4++ = *p3++;
			}

			tcl = tcl->next;
			cl = cl->next;
		}
	}*/
}

int NeuralNetwork::GetSumQnt()
{
	int ret = 0;

	for (int i = 0; i < nthreads; i++)
	{
		ret += parnetw[i]->block_size[pqnt_block_no];
	}

	return ret;
}

void NeuralNetwork::Recognize(double* _X, double* _Y)
{
	NNLayer* clevel = Wfirst;
	double* XL = NULL;
	double* YL = NULL;

	while (clevel)
	{
		if (clevel->isFirst())
		{
			if (XL)
			{
				delete XL;
			}
			XL = new double[clevel->m];
			YL = new double[clevel->n];

			double* p1 = XL;
			double* p2 = _X;
			for (int i = 0; i < in_n; i++)
			{
				*p1++ = *p2++;
			}
		}
		else
		{
			if (XL)
			{
				delete XL;
			}
			XL = new double[clevel->m];


			double* p1 = XL;
			double* p2 = YL;
			for (int i = 0; i < clevel->m; i++)
			{
				*p1++ = *p2++;
			}

			if (YL)
			{
				delete YL;
			}
			YL = new double[clevel->n];
		}

		clevel->Forward(XL, YL);

		clevel = clevel->next;
	}


	double* p1 = _Y;
	double* p2 = YL;
	for (int i = 0; i < Wlast->n; i++)
	{
		*p1++ = *p2++;
	}

	delete YL;
	delete XL;
}

int NeuralNetwork::Save(char* fname)
{
	//std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	//std::string str = utf8_conv.to_bytes(fname);

	WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, fname);
	std::ofstream* fout;

	try
	{
		fout = new std::ofstream(/*(char*)str.c_str()*/fname, std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);


		if (fout->is_open())
		{
			NN* nn;
			NNLayer* nl;

			fout->write((const char*)&in_n, sizeof(int));
			fout->write((const char*)&out_n, sizeof(int));
			fout->write((const char*)&nlayers, sizeof(int));
			fout->write((const char*)&nthreads, sizeof(int));
			fout->write((const char*)&pqnt, sizeof(int));
			fout->write((const char*)&pqnt_blocks, sizeof(int));

			//fout << _in_n << _out_n << _nlayers << _nthreads << _pqnt;
			//fout << _tracebufcnt;

			//Сохранение классов NNLayer
			nl = Wfirst;
			for (int j = 1; j < nlayers; j++)
			{
				//fout << nl->layerno << nl->n << nl->m << nl->pqnt;

				fout->write((const char*)&nl->layerno, sizeof(int));
				fout->write((const char*)&nl->m, sizeof(int));
				fout->write((const char*)&nl->n, sizeof(int));
				fout->write((const char*)&nl->pqnt, sizeof(int));

				fout->write((const char*)nl->Theta, nl->n * (nl->m + 1) * sizeof(double));
				//fout->write((const char*)nl->ThetaM, nl->n * nl->m * sizeof(double));
				fout->write((const char*)nl->ThetaT, nl->n * (nl->m + 1) * sizeof(double));
				//fout->write((const char*)nl->ThetaMT, nl->n * nl->m * sizeof(double));

				nl = nl->next;
			}

			for (int i = 0; i < nthreads; i++)
			{
				nn = parnetw[i];
				//Сохранение класса NN
				//fout << nn->in_n << nn->out_n << nn->nlayers << nn->pqnt;

				fout->write((const char*)&nn->in_n, sizeof(int));
				fout->write((const char*)&nn->out_n, sizeof(int));
				fout->write((const char*)&nn->nlayers, sizeof(int));
				fout->write((const char*)&nn->pqnt, sizeof(int));
				fout->write((const char*)&nn->pqnt_blocks, sizeof(int));

				//Сохранение классов NNLayer
				nl = nn->Wfirst;
				for (int j = 1; j < nlayers; j++)
				{
					//fout << nl->layerno << nl->n << nl->m << nl->pqnt;

					fout->write((const char*)&nl->layerno, sizeof(int));
					fout->write((const char*)&nl->m, sizeof(int));
					fout->write((const char*)&nl->n, sizeof(int));
					fout->write((const char*)&nl->pqnt, sizeof(int));

					fout->write((const char*)nl->Theta, nl->n * (nl->m + 1) * sizeof(double));
					//fout->write((const char*)nl->ThetaM, nl->n * nl->m * sizeof(double));
					fout->write((const char*)nl->ThetaT, nl->n * (nl->m + 1) * sizeof(double));
					//fout->write((const char*)nl->ThetaMT, nl->n * nl->m * sizeof(double));

					nl = nl->next;
				}
			}

			fout->close();
			return 1;
		}
	}
	catch (const std::exception& e)
	{
		char s[100];
		for (int i = 0; i < 100; i++)
		{
			s[i] = (char)0;
		}
		std::sprintf(s, "%s %s %s, %s\n", "Файл",fname, "не удалось создать", e.what());

		WriteToTraceBuffer(tracebufcnt, Tracebuffer, &tracebufw, s);
	}

	return 0;
}

char* NeuralNetwork::GetCurrentDirectory()
{
	return currentDirectory;
}

void NeuralNetwork::SetCurrentDirectory(char* dir)
{
	delete[] currentDirectory;
	currentDirectory = new char[strlength(dir) + 1];
	copystr(currentDirectory, dir, (int)strlength(dir));
}

NNDataPrepare::NNDataPrepare(int _qnt, int _n_in, int _n_out, double* x)
{
	int max_x = (_n_in + 1) * _qnt;
	//int max_y = _n_out * _qnt;
	double* p = x;

	cv = NULL;
	qnt = _qnt;
	n_in = _n_in + 1;
	n_out = _n_out;
	gmin = 99999;
	gmax = -99999;

	for (int i = 0; i < _n_in * _qnt; i++)
	{
		if (*p > gmax)
		{
			gmax = *p;
		}
		if (*p < gmin)
		{
			gmin = *p;
		}

		p++;
	}	
	
	p = x;
	X = new double[max_x];

	double* p2 = X;
	for (int i = 0; i < qnt; i++)
	{
		Normalyze(p, p2);
		p += _n_in;
		p2 += n_in;
	}	
}

NNDataPrepare::~NNDataPrepare()
{
	delete X;

	if (cv != NULL)
		delete cv;
}

void NNDataPrepare::Normalyze(double* in, double* out)
{
	int max_x = n_in - 1;
	double* _in = in;
	double* _out = out;
    double pmin = 99999;
	double pmax = -99999;
	double diff;

	for (int i = 0; i < max_x; i++)
	{
		if (*_in > pmax)
		{
			pmax = *_in;
		}
		if (*_in < pmin)
		{
			pmin = *_in;
		}

		_in++;
	}

	_in = in;

	diff = pmax - pmin;
	if (diff < 0.05 * (gmax - gmin))
	{
		diff = gmax - gmin;
	}

	for (int i = 0; i < max_x; i++)
	{
		*_out++ = (*_in++ - pmin) / diff;
	}

	*_out = (pmax - pmin) / (gmax - gmin);
}

void NNDataPrepare::EvalCVmatrix(double* x)
{
	cv = new double[n_in * n_in];
	for (int i = 0; i < n_in * n_in; i++)
	{
		cv[i] = 0;
	}

	for (int i = 0; i < qnt; i++)
	{
		double* c = cv;
		double* p1 = x + i * n_in;
		for (int j = 0; j < n_in; j++)
		{
			double* p2 = x + i * n_in;
			for (int k = 0; k < n_in; k++)
			{
				*c++ += *p1 * *p2++;
			}

			p1++;
		}
	}

	for (int i = 0; i < n_in * n_in; i++)
	{
		cv[i] /= qnt;
	}
}

void WriteToTraceBuffer(int tracebufcnt, char* tracebuffer, int* tracebufw, char* mess)
{
	if (tracebufcnt - *tracebufw > 0)
	{
		int len = (int)strlen(mess);

		if (tracebufcnt - *tracebufw > len)
		{
			for (int i = 0; i < len; i++)
			{
				*(tracebuffer + *tracebufw) = mess[i];
				(*tracebufw)++;
			}
		}
	}
}

extern "C" void Matrix8Tran(int n, int m, void* src, void* dest)
{
	matrix8tran(n, m, src, dest);
}

void memmove(char* p1, char* p2, int count)
{
	for (int i = 0; i < count; i++)
	{
		*p1++ = *p2++;
	}
}

int strlen(char* s)
{
	int ret = 0;

	while (s[ret])
	{
		ret++;
	}

	return ret;
}

/*_DLLAPI long long _stdcall nnCreate(int n, int _nlayers, int _pqnt, int _nthreads, double _rand_epsilon, int* nout, int _tracebufcnt, char* _tracebuffer)
{
	return (long long)new NeuralNetwork(n, _nlayers, _pqnt, _nthreads, _rand_epsilon, nout, _tracebufcnt, _tracebuffer);
}

_DLLAPI long long _stdcall nnLoad(char* fname, int tbcnt, char* buf)
{
	return (long long)new NeuralNetwork(fname, tbcnt, buf);
}

_DLLAPI void _stdcall nnDelete(long long nnet)
{
	NeuralNetwork* nn = (NeuralNetwork*)nnet;

	try
	{
		delete nn;
	}
	catch (const std::exception& e)
	{
		char s[100];
		for (int i = 0; i < 100; i++)
		{
			s[i] = (char)0;
		}
		sprintf_s(s, "%s %s\n", "Ошибка в функции NeuralNetwork::nnDelete", e.what());

		WriteToTraceBuffer(nn->tracebufcnt, nn->Tracebuffer, &nn->tracebufw, s);
	}
}

_DLLAPI void _stdcall nnSetPatterns(long long nnet, double* _X, double* _Y)
{
	((NeuralNetwork*)nnet)->SetPatterns(_X, _Y);
}

_DLLAPI double _stdcall nnLearn(long long nnet, int maxiter, double lambda, double epsilon, double lambdareg)
{
	NeuralNetwork* nn = (NeuralNetwork*)nnet;

	try
	{
		return nn->Learn(maxiter, lambda, epsilon, lambdareg);
	}
	catch (const std::exception& e)
	{
		char s[100];
		for (int i = 0; i < 100; i++)
		{
			s[i] = (char)0;
		}
		sprintf_s(s, "%s %s\n", "Ошибка в функции NeuralNetwork::nnLearn", e.what());

		WriteToTraceBuffer(nn->tracebufcnt, nn->Tracebuffer, &nn->tracebufw, s);
	}

	return 0;
}

_DLLAPI void _stdcall nnRecognize(long long nnet, double* _X, double* _Y)
{
	((NeuralNetwork*)nnet)->Recognize(_X, _Y);
}

_DLLAPI int _stdcall nnSave(long long nnet, wchar_t* fname)
{
	return ((NeuralNetwork*)nnet)->Save(fname);
}

_DLLAPI int _stdcall nnGetOutQnt(long long nnet)
{
	return ((NeuralNetwork*)nnet)->Wlast->n;
}

_DLLAPI long long _stdcall nndpCreate(int _qnt, int _n_in, int _n_out, double* x)
{
	NNDataPrepare* dp = new NNDataPrepare(_qnt, _n_in, _n_out, x);

	return (long long)dp;
}

_DLLAPI int _stdcall nndpGetQnt(long long dp)
{
	return ((NNDataPrepare*)dp)->n_in;
}

_DLLAPI void _stdcall nndpGetX(long long dp, double* x)
{
	double* res = ((NNDataPrepare*)dp)->X;
	double* _x = x;
	int qnt = ((NNDataPrepare*)dp)->qnt;
	int n = ((NNDataPrepare*)dp)->n_in;

	for (int i = 0; i < qnt * n; i++)
	{
		*_x++ = *res++;
	}

	return;
}

_DLLAPI double _stdcall nndpGetMax(long long dp)
{
	return ((NNDataPrepare*)dp)->gmax;
}

_DLLAPI double _stdcall nndpGetMin(long long dp)
{
	return ((NNDataPrepare*)dp)->gmin;
}

_DLLAPI void _stdcall nndpNormalyze(long long dp, double* in, double* out)
{
	((NNDataPrepare*)dp)->Normalyze(in, out);
}

_DLLAPI void _stdcall nndpDelete(long long dp)
{
	delete (NNDataPrepare*)dp;
}
*/
