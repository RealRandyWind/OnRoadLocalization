#include "NeuralNetwork.hpp"
#include <stdlib.h>
#include <stdio.h>

NeuralNetwork::NeuralNetwork(NeuralNetworkImporter* oImporter)
{
	a_ptrParameters = (unsigned char*)oImporter->lParameters;
	a_ptrConnections = (unsigned char*)oImporter->lConnections;
	a_ptrMeta = (unsigned char*)oImporter->lMeta;
	a_ptrTemp = (unsigned char*)oImporter->lTemp;
	a_ptrCustom = (unsigned char*)oImporter->lCustom;
	
	printf("_Debug: [NeuralNetwork] created NeuralNetwork.\n");
}

NeuralNetwork::~NeuralNetwork()
{

}

unsigned int* NeuralNetwork::GetConnectionsOffset(unsigned int nOffset)
{
	return (unsigned int*)(a_ptrConnections + nOffset);
}


template<typename T>
T* NeuralNetwork::GetMetaOffset(unsigned int nOffset)
{
	return (T*)(a_ptrMeta + nOffset);
}

template<typename T>
T* NeuralNetwork::GetParametersOffset(unsigned int nOffset)
{
	return (T*)(a_ptrParameters + nOffset);
}

template<typename T>
T* NeuralNetwork::GetTempOffset(unsigned int nOffset)
{
	return (T*)(a_ptrTemp + nOffset);
}

template<typename T>
T* NeuralNetwork::GetCustomOffset(unsigned int nOffset)
{
	return (T*)(a_ptrCustom + nOffset);
}