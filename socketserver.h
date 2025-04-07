#pragma once

#ifndef SOCKETSERVER_H
#define SOCKETSERVER_H

#include <cstring> 
#include <iostream>
#include <fstream>
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h> 
#include "socketfunctions.h"
#include "neuralnetw.h"
#include "parsexml.h"
#include "socketxml.h"
#include <string>
#include <vector>

#define headerBufferSize 1024
#define bufferSize 65536
#define MaxErrorQnt 32
#define MaxErrorMessLength 512
#define MyErrMessLength 128

class send_error : public std::exception
{
	std::string info;
public:
	send_error(std::string e = "Error send data") : info(e) {}
	const char* what() const noexcept {
		return info.c_str();
	}
};

class receive_error : public std::exception
{
	std::string info;
public:
	receive_error(std::string e = "Error receive data") : info(e) {}
	const char* what() const noexcept {
		return info.c_str();
	}
};

class xml_incorrect_data : public std::exception
{
	std::string info;
public:
	xml_incorrect_data(std::string e = "Incorrect data in xml-file") : info(e) {}
	const char* what() const noexcept {
		return info.c_str();
	}
};

struct DataXML
{
	bool arrayFlag = false;

	std::string TagName;
	std::vector<int> vInt;
	std::vector<double> vDouble;

	int qnt = 0; //Счётчик байт для выходной стуктуры
	int size;    //Размер выходной стуктуры в байтах
	void* data;  //Указатель на выходную структуру
};

/*
class MyErrMess
{
public:
	int errNo;
	char errMess[MyErrMessLength];

	MyErrMess(int _errNo, char* _errMess);

	~MyErrMess();
};
*/

class MySocketServerLastError
{
public:
	int GetLastErorNo();
	char* GetLastErrorMess();
	void ClearError();

private:
	int errorNo;
	char errorMess[MaxErrorMessLength];
};

class MySocketServer : public NNObject
{
public:
	MySocketServer();
	~MySocketServer();
	int startMySocketServer();
	void closeMySocketServer();
	void listenClient();
	void headerBufferZero(int size);
	void sendBuffer(int client, int qnt);
	int receiveBuffer(int client);
	bool sendBlock(int client, char* buf, int qnt);
	void sendBigData(int client, char* buf, int qnt);
	bool receiveBlock(int client, char* buf, int qnt);
	int receiveBigData(int client, char* buf);
	void copyFromBuffer(char* dest, int n); 
	void copyToBuffer(char* src, int n);

	void nnCreate(int client);
	void nnLoad(int client);
	void nnDelete(int client);
	//void nnSetPatterns(int client);
	void nnSetPatternsFromFiles(int client);
	double nnLearn(int client);
	void nnRecognize(int client);
	int nnSave(int client);
	void nnGetOutQnt(int client);
	void nnGetCurrentDirectory(int client);
	void nnSetCurrentDirectory(int client);
	void nnWriteFile(int client);
	void nnReadFile(int client);
	void nnGetLastError(int client);
	void nnWriteFileBlock(int client);

	void nndpCreate(int client);
	void nndpGetQnt(int client);
	void nndpGetX(int client);
	void nndpGetMax(int client);
	void nndpGetMin(int client);
	void nndpNormalyze(int client);
	void nndpDelete(int client);
	void gauss(int client);

	//unsigned short port;
private:
	int socketID;

	NeuralNetwork* nnetw;

	int fSize;
	char* fData;

	//Буферы
	char headerBuffer[headerBufferSize];
	char dataBuffer[bufferSize];

	char lastEror[MyErrMessLength];
};

#endif
