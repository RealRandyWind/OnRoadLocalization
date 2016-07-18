#ifndef NEURALNETWORK_HPP
#define NEURALNETWORK_HPP
#include "NeuralNetworkImporter.hpp"
#include <vector>

#define NEURALNETWORK_EPS 0.001f

class NeuralNetwork;
class NeuralNetwork
{
private:
	unsigned char* a_ptrParameters;
	unsigned char* a_ptrTemp;
	unsigned char* a_ptrMeta;
	unsigned char* a_ptrConnections;
	unsigned char* a_ptrCustom;
public:
	NeuralNetwork(NeuralNetworkImporter* oImporter);
	~NeuralNetwork();

	virtual void* Use(void* lInputs, void* lOutputs) = 0;
	virtual void* Validate(void* lSamples, void* lSampleTargets, void* lReturns, unsigned int nSamples) = 0;
	virtual void Train(void* lSamples, void* lSampleTargets, unsigned int nSamples, void* lTrainingParms, unsigned int nTraningParms) = 0;
	virtual void Optimize(void* lOptimizationParms, unsigned int nOptimizationParms) = 0;

	unsigned int* GetConnectionsOffset(unsigned int nOffset);

	template<typename T = void>
	T* GetMetaOffset(unsigned int nOffset);
	template<typename T = void>
	T* GetParametersOffset(unsigned int nOffset);
	template<typename T = void>
	T* GetTempOffset(unsigned int nOffset);
	template<typename T = void>
	T* GetCustomOffset(unsigned int nOffset);
};
#endif
