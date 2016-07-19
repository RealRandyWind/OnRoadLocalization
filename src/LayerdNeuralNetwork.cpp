#include "LayerdNeuralNetwork.hpp"

LayerdNeuralNetwork::LayerdNeuralNetwork(
		DataTypesLayerd::DObjectInitializer oInitializer,
		Allocator* oCustomAllocator
	) : NeuralNetwork()
{
	a_oAllocator = oCustomAllocator;
	a_eErrorCode = 0;
	a_nLayers = 0;
	a_nLayersAllocated = 0;
	a_oInputLayer = nullptr;
	a_lLayers = nullptr;
	a_oNeurons = nullptr;
	a_oOutputLayer = nullptr;

	a_oInput = nullptr;
	a_oCharge = nullptr;
	a_oTemp = nullptr;
	a_oOutput = nullptr;

	void* ptrState = 0;
	unsigned int nBytesState = oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge)
		+ oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge)
		+ oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DNeuron)
		+ oInitializer.nHidden * oInitializer.nInput * sizeof(DataTypesLayerd::DInputSynapse)
		+ oInitializer.nHidden * oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DSynapse)
		+ oInitializer.nHidden * oInitializer.nOutput * sizeof(DataTypesLayerd::DOutputSynapse);

	if (oInitializer.ptrState)
	{
		ptrState = oInitializer.ptrState;
		a_eErrorCode = (oInitializer.nBytesState != nBytesState);
		nBytesState = oInitializer.nBytesState;
	}
	else { ptrState = a_oAllocator->Allocate(nBytesState); }

	SetState(ptrState, nBytesState, oInitializer);
}

LayerdNeuralNetwork::~LayerdNeuralNetwork()
{

}

void LayerdNeuralNetwork::ReInitialize(
		void* lState,
		DataTypesLayerd::DObjectInitializer oInitializer
	)
{
	a_nLayers = oInitializer.nLayers;
	/* compute offsets in order */
	unsigned int nBytesTempOffset = 0;
	unsigned int nBytesChargeOffset = oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge) + nBytesTempOffset;;
	unsigned int nBytesNeuronsOffset = oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge) + nBytesChargeOffset;
	unsigned int nBytesInputOffset = oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DNeuron) + nBytesNeuronsOffset;
	unsigned int nBytesSynapseLayerOffset = oInitializer.nInput * oInitializer.nHidden * sizeof(DataTypesLayerd::DInputSynapse) + nBytesInputOffset;
	unsigned int nBytesOutputOffset = oInitializer.nHidden * oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DSynapse) + nBytesSynapseLayerOffset;

	/* create input, output placeholders */
	if (!a_oInput) { a_oInput = new DataVector<DataTypesLayerd::DInput>(1, oInitializer.nInput, a_oAllocator, true); }
	if (!a_oOutput) { a_oOutput = new DataVector<DataTypesLayerd::DOutput>(1, oInitializer.nOutput, a_oAllocator, true); }

	/* create temporary result vector */
	if (!a_oCharge) { a_oCharge = new DataVector<DataTypesLayerd::DCharge>(1, oInitializer.nHidden, a_oAllocator, true); }
	if (!a_oTemp) { a_oTemp = new DataVector<DataTypesLayerd::DCharge>(1, oInitializer.nHidden, a_oAllocator, true); }
	a_oCharge->Place(GetState<DataTypesLayerd::DCharge>(nBytesChargeOffset, true));
	a_oTemp->Place(GetState<DataTypesLayerd::DCharge>(nBytesTempOffset, true));

	/* create parameters for the neurons */
	if (!a_oNeurons) { a_oNeurons = new DataVector<DataTypesLayerd::DNeuron>(1, oInitializer.nHidden, a_oAllocator, true); }
	a_oNeurons->Place(GetState<DataTypesLayerd::DNeuron>(nBytesNeuronsOffset, true));

	/* allocate layers */
	if (!a_oInputLayer) { a_oInputLayer = new DataVector<DataTypesLayerd::DInputSynapse>(oInitializer.nInput, oInitializer.nHidden, a_oAllocator, true); }
	if (oInitializer.nLayers > a_nLayersAllocated) 
	{ 
		a_lLayers = a_oAllocator->Allocate<DataVector<DataTypesLayerd::DSynapse>>(oInitializer.nLayers);
		a_nLayersAllocated = oInitializer.nLayers;
	}
	if (!a_oOutputLayer) { a_oOutputLayer = new DataVector<DataTypesLayerd::DSynapse>(oInitializer.nOutput, oInitializer.nHidden, a_oAllocator, true); }

	/* create parameters for the input */
	a_oInputLayer->Place(GetState<DataTypesLayerd::DSynapse>(nBytesInputOffset, true));

	for (unsigned int iLayer = 0; iLayer < oInitializer.nLayers; iLayer++)
	{
		/* create parameters for layer */
		a_lLayers[iLayer] = DataVector<DataTypesLayerd::DSynapse>(oInitializer.nHidden, oInitializer.nHidden, a_oAllocator, true);
		a_lLayers[iLayer].Place(GetState<DataTypesLayerd::DSynapse>(nBytesSynapseLayerOffset, true));
		nBytesSynapseLayerOffset += sizeof(DataTypesLayerd::DSynapse) * oInitializer.nHidden;
	}

	/* create parameters for the output */
	a_oOutputLayer->Place(GetState<DataTypesLayerd::DSynapse>(nBytesOutputOffset, true));
}

DataTypesLayerd::DOutput* LayerdNeuralNetwork::Use(
		DataTypesLayerd::DInput* lInputs,
		DataTypesLayerd::DOutput* lOutputs
	)
{
	a_oInput->Place(lInputs);
	a_oOutput->Place(lOutputs);

	a_oInputLayer->Mul(a_oInput, a_oCharge);
	for (unsigned int iLayer = 0; iLayer < a_nLayers; iLayer++)
	{
		a_lLayers[iLayer]
			.Mul(a_oCharge, a_oTemp)
			->Add(a_oNeurons, a_oCharge);
	}
	a_oOutputLayer->Mul(a_oCharge, a_oOutput);

	return a_oOutput->Data();
}

DataTypesLayerd::DOutput* LayerdNeuralNetwork::Validate(
		DataTypesLayerd::DInput* lSamples,
		DataTypesLayerd::DOutput* lSampleTargets,
		DataTypesLayerd::DOutput* lReturns,
		unsigned int nSamples,
		DataTypesLayerd::DValidation oValidationParms
	)
{
	return nullptr;
}

void LayerdNeuralNetwork::Train(
		DataTypesLayerd::DInput* lSamples,
		DataTypesLayerd::DOutput* lSampleTargets,
		unsigned int nSamples,
		DataTypesLayerd::DTraining oTrainingParms
	)
{

}

void LayerdNeuralNetwork::Optimize(
		DataTypesLayerd::DOptimization oOptimizationParms
	)
{

}
