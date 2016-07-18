#ifndef LAYERDNEURALNETWORK_HPP
#define LAYERDNEURALNETWORK_HPP

#include "stdint.h"
#include "NeuralNetwork.hpp"
#include "Allocator.hpp"
#include "DataVector.hpp"

#include "LayerdNeuralNetwork.datatypes.hpp"

class LayerdNeuralNetwork;
class LayerdNeuralNetwork : 
	public NeuralNetwork<
		DataTypesLayerd::DInput,
		DataTypesLayerd::DOutput,
		DataTypesLayerd::DValidation,
		DataTypesLayerd::DTraining,
		DataTypesLayerd::DOptimization,
		DataTypesLayerd::DObjectInitializer
	>
{
private:
	Allocator* a_oAllocator;

	DataVector<DataTypesLayerd::DInput>* a_oInput;
	DataVector<DataTypesLayerd::DOutput>* a_oOutput;
	DataVector<DataTypesLayerd::DCharge>* a_oCharge;
	DataVector<DataTypesLayerd::DCharge>* a_oTemp;

	DataVector<DataTypesLayerd::DNeuron>* a_oNeurons;
	DataVector<DataTypesLayerd::DInputSynapse>* a_oInputLayer;
	DataVector<DataTypesLayerd::DSynapse>* a_lLayers;
	DataVector<DataTypesLayerd::DOutputSynapse>* a_oOutputLayer;

	unsigned int a_nLayers;
	unsigned int a_nLayersAllocated;
public:
	LayerdNeuralNetwork(
			DataTypesLayerd::DObjectInitializer oInitializer,
			Allocator* oCustomAllocator = Allocator::GetDefault()
		);

	~LayerdNeuralNetwork();

	void ReInitialize(
			void* lState,
			DataTypesLayerd::DObjectInitializer oInitializer
		);

	virtual DataTypesLayerd::DOutput* Use(
			DataTypesLayerd::DInput* lInputs, 
			DataTypesLayerd::DOutput* lOutputs
		) override;

	virtual DataTypesLayerd::DOutput* Validate(
			DataTypesLayerd::DInput* lSamples, 
			DataTypesLayerd::DOutput* lSampleTargets, 
			DataTypesLayerd::DOutput* lReturns, 
			unsigned int nSamples, 
			DataTypesLayerd::DValidation oValidationParms
		) override;

	virtual void Train(
			DataTypesLayerd::DInput* lSamples, 
			DataTypesLayerd::DOutput* lSampleTargets, 
			unsigned int nSamples, 
			DataTypesLayerd::DTraining oTrainingParms
		) override;

	virtual void Optimize(
			DataTypesLayerd::DOptimization oOptimizationParms
		) override;
};
#endif
