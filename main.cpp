#include <cstdio>
#include "socketserver.h"
#include "socketfunctions.h"
#include "neuralnetw.h"

//using namespace std;

void test()
{
    int nout[3] = { 25, 10 };
    //char* tb = new char[200000];

    //NeuralNetwork* nn = new NeuralNetwork(400, 3, 5000, 10, 2, 0.12, nout, 200000, tb, OPTIMIZATION_ASM64);
    //delete tb;
}

int main()
{
    printf("Вас приветствует %s!\n", "mathserver");
    //test();
    
    MySocketServer* myserv = new MySocketServer();
    if (myserv->startMySocketServer() == -1)
    {
        return 0;
    }
    myserv->listenClient();
    myserv->closeMySocketServer();

    return 0;
}
