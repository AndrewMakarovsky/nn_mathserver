#include "socketserver.h"
#include <arpa/inet.h>
#include "neuralnetw.h"

using namespace std;
using namespace socketxml;

MySocketServer::MySocketServer() : NNObject()
{
	nnetw = NULL;

	for (int i = 0; i < MyErrMessLength; i++)
	{
		lastEror[i] = 0;
	}
}

MySocketServer::~MySocketServer()
{
}

int MySocketServer::startMySocketServer()
{
	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if (socketID < 0)
	{
		cout << "Socket error " << errno << endl;
		return -1;
	}

	sockaddr_in socketAddr;
	socketAddr.sin_family = AF_INET;
	socketAddr.sin_port = htons(((NNObject*)this)->getIniFile()->portno);
	socketAddr.sin_addr.s_addr = INADDR_ANY/*inet_addr(((NNObject*)this)->getIniFile()->ipaddr.c_str())*/;

	cout << "Port no: " << ((NNObject*)this)->getIniFile()->portno << endl;

	if (bind(socketID, (struct sockaddr*)&socketAddr, sizeof(socketAddr)) < 0)
	{
		cout << "Socket bind error " << errno << endl;

		return -1;
	}
}

void MySocketServer::listenClient()
{
	bool flag = true;
	int qnt = 0;

	while (flag)
	{
		int clientSocket;
		int commandNo;

		listen(socketID, 5);
		try
		{
			clientSocket = 0;
		    clientSocket = accept(socketID, nullptr, nullptr);
		    headerBufferZero(sizeof(int));
		    recv(clientSocket, headerBuffer, sizeof(int), 0);
		    commandNo = *(int*)headerBuffer;
		}
		catch(exception& e)
		{
			cout << "Command: " << commandNo << ", Error: " << e.what() << endl;
			copystr(lastEror, e.what(), MyErrMessLength);
			if (clientSocket > 0) {close(clientSocket);}
		}

		//Если вызываемая функция имеет входные параметры - читается буфер, который их содержит,
		//затем большой буфер данных, если он есть.
		//После завершения выполнения функции возвращается буфер, содержащий результат.
		try
		{
		switch (commandNo)
		{
		case closeMySocketServer_Function:
			close(clientSocket);
			flag = false;
			break;

		case nnCreate_Function:
			nnCreate(clientSocket);
			break;

		case nnLoad_Function:
			nnLoad(clientSocket);
			break;

		case nnDelete_Function:
			nnDelete(clientSocket);
			break;

		case nnSetPatterns_Function:
			//nnSetPatterns(clientSocket);
			break;

		case nnSetPatternsFromFiles_Function:
			nnSetPatternsFromFiles(clientSocket);
			break;

		case nnLearn_Function:
			nnLearn(clientSocket);
			break;

		case nnRecognize_Function:
			nnRecognize(clientSocket);
			break;

		case nnSave_Function:
			nnSave(clientSocket);
			break;

		case nnGetOutQnt_Function:
			nnGetOutQnt(clientSocket);
			break;

		case nnGetCurrentDirectory_Function:
			nnGetCurrentDirectory(clientSocket);
			break;

		case nnSetCurrentDirectory_Function:
			nnSetCurrentDirectory(clientSocket);
			break;

		case nnReadFile_Function:
			break;

		case nnWriteFile_Function:
			nnWriteFile(clientSocket);
			break;

		case nnWriteFileBlock_Function:
			nnWriteFileBlock(clientSocket);
			break;

		case nndpCreate_Function:
			nndpCreate(clientSocket);
			break;

		case nndpGetQnt_Function:
			break;

		case nndpGetX_Function:
			break;

		case nndpGetMax_Function:
			break;

		case nndpGetMin_Function:
			break;

		case nndpNormalyze_Function:
			break;

		case nndpDelete_Function:
			break;

		case gauss_Function:
			break;

		case nnGeLastError_Function:
			nnGetLastError(clientSocket);
			break;

		case testsocketserver_Function:
			//Считываем входные данные 
			qnt = receiveBuffer(clientSocket);

			//И отдаем в ответ те же данные.
			sendBuffer(clientSocket, qnt);

			break;

		//default:
		//	break;
		}
		}
		catch(exception &e)
		{
			cout << "Command: " << commandNo << ", Error: " << e.what() << endl;
			copystr(lastEror, e.what(), MyErrMessLength);
			close(clientSocket);
		}
	}
}

void MySocketServer::sendBuffer(int client, int qnt)
{
	int rqnt;

	rqnt = send(client, (void*)&qnt, sizeof(int), 0);
	if (rqnt != sizeof(int))
	{
		throw send_error();
	}
	rqnt = send(client, (void*)dataBuffer, qnt, 0);
	if (rqnt != qnt)
	{
		throw send_error();
	}
}

int MySocketServer::receiveBuffer(int client)
{
	int qnt, rqnt;

	rqnt = recv(client, (void*)&qnt, sizeof(int), 0);
	if (rqnt != sizeof(int))
	{
		throw receive_error();
	}
	rqnt = recv(client, (void*)dataBuffer, qnt, 0);
	if (rqnt != qnt)
	{
		throw receive_error();
	}

	return qnt;
}

bool MySocketServer::sendBlock(int client, char* buf, int qnt)
{
	int rqnt, rqnt2;

	send(client, (char*)&qnt, sizeof(int), 0);
	send(client, (char*)buf, qnt, 0);
	rqnt2 = recv(client, (char*)&rqnt, sizeof(int), 0);
	if (rqnt2 != sizeof(int))
	{
		throw send_error();
	}
	else
	{
		if (rqnt != qnt)
		{
			return false;
		}
	}

	return true;
}

void MySocketServer::sendBigData(int client, char* buf, int qnt)
{
	char* p = buf;

	int nblocks = qnt / bufferSize;
	int lastblock = qnt % bufferSize;

	send(client, (char*)&qnt, sizeof(int), 0);

	for (int i = 0; i < nblocks; i++)
	{
		int count = 0;
		while (!sendBlock(client, p, bufferSize) && count < 3)
		{
			count++;
		}

		if (count >= 3)
		{
			throw send_error();
		}

		p += bufferSize;
	}

	if (lastblock > 0)
	{
		int count = 0;
		while (!sendBlock(client, p, lastblock) && count < 3)
		{
			count++;
		}
		if (count >= 3)
		{
			throw send_error();
		}
	}
}

bool MySocketServer::receiveBlock(int client, char* buf, int qnt)
{
	int rqnt;

	rqnt = recv(client, (char*)&qnt, sizeof(int), 0);
	rqnt = recv(client, (char*)buf, qnt, 0);
	if (rqnt != sizeof(int))
	{
		throw receive_error();
	}
	else
	{
		if (rqnt != qnt)
		{
			return false;
		}
	}
	send(client, (char*)&rqnt, sizeof(int), 0);

	return true;
}

int MySocketServer::receiveBigData(int client, char* buf)
{
	int qnt;
	char* p = buf;

	recv(client, (char*)&qnt, sizeof(int), 0);

	int nblocks = qnt / bufferSize;
	int lastblock = qnt % bufferSize;

	for (int i = 0; i < nblocks; i++)
	{
		int count = 0;
		while (!receiveBlock(client, p, bufferSize) && count < 3)
		{
			count++;
		}
		if (count >= 3)
		{
			throw receive_error();
		}
		p += bufferSize;
	}

	if (lastblock > 0)
	{
		int count = 0;
		while (!receiveBlock(client, p, lastblock) && count < 3)
		{
			count++;
		}
		if (count >= 3)
		{
			throw receive_error();
		}
	}

	return qnt;
}

void MySocketServer::closeMySocketServer()
{
	close(socketID);
}

void MySocketServer::headerBufferZero(int size)
{
	char* p = headerBuffer;

	if (size > headerBufferSize || size < 0)
		return;

	for (int i = 0; i < size; i++)
	{
		*p++ = 0;
	}
}

void MySocketServer::copyFromBuffer(char* dest, int n)
{
	char* p = dataBuffer;

	for (int i = 0; i < n; i++)
	{
		*dest++ = *p++;
	}
}

void MySocketServer::copyToBuffer(char* src, int n)
{
	char* p = dataBuffer;

	for(int i = 0; i < bufferSize; i++)
	{
		*p++ = 0;
	}

	p = dataBuffer;

	for (int i = 0; i < n; i++)
	{
		*p++ = *src++;
	}
}

void MySocketServer::nnCreate(int client)
{
	long long ret = 0;
	char* xml = new char[1024];
	nnCreateParms parms;
	char* tracebuffer = NULL;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;

	if (parms.tracebufcnt > 0)
	{
		tracebuffer = new char[parms.tracebufcnt];
		char* p = tracebuffer;

		for (int i = 0; i < parms.tracebufcnt; i++)
		{
			*p++ = 0;
		}
	}

	nnetw = new NeuralNetwork(parms.n, parms.nlayers, parms.pqnt, parms.pqnt_blocks, parms.nthreads, parms.rand_epsilon, parms.nout, parms.tracebufcnt, tracebuffer, parms.optimization);

	delete[] parms.nout;

	ret = (long long)nnetw;
	copyToBuffer((char*)&ret, sizeof(long long));
	sendBuffer(client, sizeof(long long));
}

void MySocketServer::nnLoad(int client)
{
	long long ret = 0;
	char* xml = new char[1024];
	nnLoadParms parms;
	char* tracebuf;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	tracebuf = new char[parms.tbcnt];
	char* p = tracebuf;

	for (int i = 0; i < parms.tbcnt; i++)
	{
		*p++ = 0;
	}

	nnetw = new NeuralNetwork((char*)parms.fname, parms.tbcnt, tracebuf, parms.optimization);

	delete[] parms.fname;

	ret = (long long)nnetw;
	copyToBuffer((char*)&ret, sizeof(long long));
	sendBuffer(client, sizeof(long long));
}

void MySocketServer::nnDelete(int client)
{
	nnDeleteParms parms;
	char* xml = new char[1024];

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	int sz = nn->tracebufcnt;
	copyToBuffer(nn->Tracebuffer, sz);
	sendBuffer(client, sz);

	delete nn->Tracebuffer;
	delete nn;
	nnetw = NULL;
}
/*
void MySocketServer::nnSetPatterns(int client)
{
	nnSetPatternsParms parms;
	char* xml = new char[1024];
	double* _X;
	double* _Y;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;
	if (parms.n != nn->in_n * nn->pqnt || parms.m != nn->out_n * nn->pqnt)
	{
		throw xml_incorrect_data();
	}

	_X = new double[parms.n];
	qnt = read(client, (char*)_X, parms.n * sizeof(double));
	if (qnt != parms.n * sizeof(double))
	{
		delete _X;
		throw receive_error();
	}
	
	_Y = new double[parms.m];
	qnt = read(client, (char*)_Y, parms.m * sizeof(double));
	if (qnt != parms.m * sizeof(double))
	{
		delete _Y;
	    delete _X;
		throw receive_error();
	}

	delete _Y;
	delete _X;
}*/

void MySocketServer::nnWriteFileBlock(int client)
{
	nnWriteFileBlockParms parms;
	char* xml = new char[1024];

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;

	cout << "blockno: " << parms.blockno << ", blocksize: " << parms.blocksize << endl;

	if (parms.blockqnt < 1 || parms.blockqnt <= parms.blockno)
	{
		throw xml_incorrect_data();
	}

	if (parms.blockqnt == 1 && parms.filesize != parms.blocksize)
	{
		throw xml_incorrect_data();
	}

	if (parms.blockqnt > 1 && parms.filesize != parms.blocksize * (parms.blockqnt - 1) + parms.lastblocksize)
	{
		throw xml_incorrect_data();
	}

    if (fSize != parms.filesize)
	{
		fSize = parms.filesize;
		fData = new char[parms.filesize];
		char* p = fData;
		for(int i = 0; i < parms.filesize; i++)
		{
			*p++ = 0;
		}
	}

	if (parms.blockno == parms.blockqnt - 1)
	{
		qnt = parms.lastblocksize;
	}
	else
	{
		qnt = parms.blocksize;
	}

    int rqnt = recv(client, fData + parms.blocksize * parms.blockno, qnt, 0);
	if (rqnt != qnt)
	{
		cout << "rqnt, qnt: " << rqnt << ", " << qnt << endl;
		throw receive_error();
	}
}

void MySocketServer::nnSetPatternsFromFiles(int client)
{
	nnSetPatternsFromFilesParms parms;
	char* xml = new char[1024];
	double* X;
	double* Y;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;
	if (parms.n != nn->in_n * nn->pqnt || parms.m != nn->out_n * nn->pqnt)
	{
		throw xml_incorrect_data();
	}

	int n = nn->in_n * nn->pqnt;
	int m = nn->out_n * nn->pqnt;
	X = new double[n];
	ifstream in(parms.f1, ios_base::binary);
	if (in.is_open())
	{
		in.read((char*)X, n * sizeof(double));
	}

	in.close();

	Y = new double[m];
	ifstream in2(parms.f2, ifstream::binary);
	if (in2.is_open())
	{
		in2.read((char*)Y, m * sizeof(double));
	}

	in2.close();

	nn->SetPatterns(X, Y);

	delete Y;
	delete X;
}

double MySocketServer::nnLearn(int client)
{
	nnLearnResultParms ret;
	char* xml = new char[1024];
	nnLearnParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	ret = nn->Learn(parms.maxiter, parms.lambda, parms.epsilon, parms.lambdareg);

	int sz = sizeof(ret);
	copyToBuffer((char*)&ret, sz);
	sendBuffer(client, sz);

	return ret.norma;
}

void MySocketServer::nnRecognize(int client)
{
	nnRecognizeParms parms;
	char* xml = new char[1024];
	double* X;
	double* Y;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;

	NeuralNetwork* nnet = (NeuralNetwork*)parms.nnet;
	if (parms.n != nnet->in_n || parms.m != nnet->out_n)
	{
		throw xml_incorrect_data();
	}

	X = new double[nnet->in_n];
	Y = new double[nnet->out_n];

	qnt = receiveBuffer(client);
	if (qnt != parms.n * sizeof(double))
	{
		throw xml_incorrect_data();
	}
	copyFromBuffer((char*)X, qnt);

	nnet->Recognize(X, Y);
	copyToBuffer((char*)Y, nnet->out_n * sizeof(double));
	sendBuffer(client, nnet->out_n * sizeof(double));

	delete Y;
	delete X;
}

int MySocketServer::nnSave(int client)
{
	int ret = 0;
	char* xml = new char[1024];
	nnSaveParms parms;	

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	if (nn != NULL)
	{
		ret = nn->Save(parms.fname);
	}

	return ret;
}

void MySocketServer::nnGetOutQnt(int client)
{
	int ret = 0;
	char* xml = new char[1024];
	nnGetOutQntParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	if (nn != NULL)
	{
		ret = nn->out_n;
	}
	
	copyToBuffer((char*)&ret, sizeof(int));
	sendBuffer(client, sizeof(int));
}

void MySocketServer::nnGetLastError(int client)
{
	int ret = strlength(lastEror);
	int rqnt = send(client, (void*)&ret, sizeof(int), 0);
	if (rqnt != sizeof(int))
	{
		throw send_error();
	}

	if (ret > 0)
	{
		rqnt = send(client, (void*)dataBuffer, ret, 0);
		if (rqnt != ret)
	    {
		    throw send_error();
	    }
	}
}

void MySocketServer::nnGetCurrentDirectory(int client)
{
	char* xml = new char[1024];
	nnGetCurrentDirectoryParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	if (nn != NULL)
	{
		CreateXML cxml;
		cxml.addString(nn->currentDirectory);
		copystr(xml, cxml.getXML().c_str(), 1024);
	}

	copyToBuffer(xml, (int)strlength(xml));
	sendBuffer(client, (int)strlength(xml));

	delete[] xml;
}

void MySocketServer::nnSetCurrentDirectory(int client)
{
	char* xml = new char[1024];
	nnSetCurrentDirectoryParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	if (nn != NULL)
	{
		nn->SetCurrentDirectory(parms.dir);
		delete[] parms.dir;
	}
}

void MySocketServer::nnWriteFile(int client)
{
	char* xml = new char[1024];
	nnWriteFileParms parms;
	char* X;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;

	if (parms.n != fSize)
	{
		cout << "parms.n, fSize: " << parms.n << ", " << fSize << endl;
		throw xml_incorrect_data();
	}

	ofstream in(parms.fname, ios_base::binary);

	delete[] parms.fname;

	if (in.is_open())
	{
		in.write((char*)fData, fSize);
		in.close();

		delete[] fData;
		fSize = 0;
	}
}

void MySocketServer::nnReadFile(int client)
{
	char* xml = new char[1024];
	nnReadFileParms parms;
	char* X;
	int n = 0;

	int qnt = receiveBuffer(client);
	copyFromBuffer(xml, qnt);
	myXML myxml(&parms, sizeof(parms));
	parsexml::ParseXML pf(otf, ctf, ttf, &myxml);
	string s_xml(xml);
	string err;
	pf.parse(s_xml, err);
	delete[] xml;
	NeuralNetwork* nn = (NeuralNetwork*)parms.nnet;

	if (nn != NULL)
	{
		ifstream in(parms.fname, ios_base::binary);
		if (in.is_open())
		{
			in.seekg(0, ios::end);
			n = in.tellg();

			X = new char[n];
			in.read((char*)X, n);

			in.close();

			sendBigData(client, X, n);

			delete[] X;
		}
	}

	delete[] parms.fname;
}

void MySocketServer::nndpCreate(int client)
{
	long long ret = 0;
	double* _X;
	nndpCreateParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	_X = new double[parms.n_in * parms.qnt];

	receiveBigData(client, (char*)_X);

	ret = (long long)new NNDataPrepare(parms.qnt, parms.n_in, parms.n_out, _X);
	copyToBuffer((char*)&ret, sizeof(long long));
	sendBuffer(client, sizeof(long long));

	delete _X;
}

void MySocketServer::nndpGetQnt(int client)
{
	int ret = 0;

	nndpGetQntParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;

	if (dp != NULL)
	{
		ret = dp->qnt;
	}

	copyToBuffer((char*)&ret, sizeof(int));
	sendBuffer(client, sizeof(int));
}

void MySocketServer::nndpGetX(int client)
{
	nndpGetXParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;

	sendBigData(client, (char*)dp->X, dp->n_in * dp->qnt);
}

void MySocketServer::nndpGetMax(int client)
{
	double ret = 0;

	nndpGetMaxParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;

	if (dp != NULL)
	{
		ret = dp->gmax;
	}

	copyToBuffer((char*)&ret, sizeof(double));
	sendBuffer(client, sizeof(double));
}
void MySocketServer::nndpGetMin(int client)
{
	double ret = 0;

	nndpGetMinParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;

	if (dp != NULL)
	{
		ret = dp->gmin;
	}

	copyToBuffer((char*)&ret, sizeof(double));
	sendBuffer(client, sizeof(double));
}

void MySocketServer::nndpNormalyze(int client)
{
	double* in;
	double* out;
	nndpNormalyzeParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);

	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;
	
	in = new double[dp->n_in * dp->qnt];
	out = new double[dp->n_out * dp->qnt];

	qnt = receiveBigData(client, (char*)in);

	dp->Normalyze(in, out);

	sendBigData(client, (char*)out, (dp->n_in + 1) * dp->qnt * sizeof(double));

	delete out;
	delete in;
}

void MySocketServer::nndpDelete(int client)
{
	nndpDeleteParms parms;

	int qnt = receiveBuffer(client);
	copyFromBuffer((char*)&parms, qnt);
	NNDataPrepare* dp = (NNDataPrepare*)parms.dp;

	delete dp;
}

void MySocketServer::gauss(int client)
{

}

/*void MySocketServer::getLastError(int client)
{
	int ret = lastEror.GetLastErorNo();

	copyToBuffer((char*)&ret, sizeof(int));
	sendBuffer(client, sizeof(int));

	if (ret > 0)
	{
		ret = strlen(lastEror.GetLastErrorMess()) + 1;
		copyToBuffer((char*)lastEror.GetLastErrorMess(), ret);
		sendBuffer(client, ret);
	}
}*/

int MySocketServerLastError::GetLastErorNo()
{
	return errorNo;
}

char* MySocketServerLastError::GetLastErrorMess()
{
	return errorMess;
}

void MySocketServerLastError::ClearError()
{
	char* p = errorMess;
	errorNo = 0;

	for (int i = 0; i < MaxErrorMessLength; i++)
	{
		*p++ = 0;
	}
}

