#include "stdafx.h"
#include "CppUnitTest.h"
#include "LayerdNeuralNetwork.hpp"
#include "Allocator.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	#define ABS(V) (V) < 0 ? -(V) : (V);
	#define TEST_EPS 0.001f
	#define TEST_ONE 1
	#define TEST_TWO 2
	#define TEST_THREE 3
	#define TEST_FEW 16
	#define TEST_INITIALIZER(N) {(N),(N),2 * (N),(N), 0, nullptr}
	#define TEST_INIT_INPUT { 255, 255, 255, 255 }
	#define TEST_INIT_SYNAPSE(N) { 1.0/(N), 1.0/(N), 1.0/(N)}
	#define TEST_INIT_NEURON(N) { 1.0/(N) }
	#define TEST_INIT_OUTPUT { 1.0 }

	TEST_CLASS(TestLayerdNeuralNetwork)
	{
	public:
		DataTypesLayerd::DInput* Fx_InitializeInput(DataTypesLayerd::DInput* lInputs, unsigned int nInputs)
		{
			unsigned int iIndex = nInputs;
			while (iIndex--) { lInputs[iIndex] = TEST_INIT_INPUT; }
			return lInputs;
		}
		
		template<typename T>
		T* Fx_Offset(void* ptrState, unsigned int nOffset)
		{
			return(T*)((unsigned char*)ptrState + nOffset);
		}

		void* Fx_InitializeState(void* ptrState, DataTypesLayerd::DObjectInitializer oInitializer)
		{
			unsigned int nBytesTempOffset = 0;
			unsigned int nBytesChargeOffset = oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge) + nBytesTempOffset;;
			unsigned int nBytesNeuronsOffset = oInitializer.nHidden * sizeof(DataTypesLayerd::DCharge) + nBytesChargeOffset;
			unsigned int nBytesInputOffset = oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DNeuron) + nBytesNeuronsOffset;
			unsigned int nBytesSynapseLayerOffset = oInitializer.nInput * oInitializer.nHidden * sizeof(DataTypesLayerd::DInputSynapse) + nBytesInputOffset;
			unsigned int nBytesOutputOffset = oInitializer.nHidden * oInitializer.nHidden * oInitializer.nLayers * sizeof(DataTypesLayerd::DSynapse) + nBytesSynapseLayerOffset;
	
			unsigned int iIndex;
			
			DataTypesLayerd::DSynapse* lSynapses = Fx_Offset(ptrState, nBytesSynapseLayerOffset);
			iIndex = oInitializer.nLayers * oInitializer.nHidden * oInitializer.nHidden;
			while(iIndex--)
			{
				lSynapses[iIndex] = TEST_INIT_SYNAPSE(oInitializer.nHidden + 1)
			}
			
			DataTypesLayerd::DNeuron* lNeurons = Fx_Offset(ptrState, nBytesNeuronsOffset);
			iIndex = oInitializer.nLayers * oInitializer.nHidden;
			while(iIndex--)
			{
				lNeurons[iNeurons] = TEST_INIT_NEURON(oInitializer.nHidden + 1)
			}
			
			DataTypesLayerd::DOutputSynapse* lOutputSynapses = Fx_Offset(ptrState, nBytesOutputOffset);
			iIndex = oInitializer.nHidden * oInitializer.nOutput;
			while(iIndex--)
			{
				lOutputSynapses[iIndex] = TEST_INIT_SYNAPSE(3 * (oInitializer.nHidden + 1))
			}
			
			DataTypesLayerd::DInputSynapse* lInputSynapses = Fx_Offset(ptrState, nBytesInputOffset);
			iIndex = oInitializer.nHidden * oInitializer.nInput;
			while(iIndex--)
			{
				lInputSynapses[iIndex] = TEST_INIT_SYNAPSE(oInitializer.nInput)
			}
			
			return ptrState;
		}

		DataTypesLayerd::DOutput* Fx_InitialzeExpectedOutput(DataTypesLayerd::DOutput* lOutputs, unsigned int nOutputs)
		{
			unsigned int iIndex = nOutputs;
			while (iIndex) {
				lOutputs[iIndex] = TEST_INIT_OUTPUT;
			}
			return lOutputs;
		}
		
		void Fx_AssertOutput(DataTypesLayerd::DOutput* lOutputs,unsigned int nOutputs, DataTypesLayerd::DOutput oExpected)
		{
			unsigned int iIndex = nOutputs;
			while (iIndex) {
				DataTypesLayerd::DOutput oCurrent = lOutputs[iIndex];
				float dError = ABS(oCurrent.dProbability - oExpected.dProbability);
				Assert::IsTrue(dError < TEST_EPS);
			}
		}

		TEST_METHOD(TestLayerdNeuralNetworkConstructor)
		{
			LayerdNeuralNetwork* oNetwork = new LayerdNeuralNetwork(TEST_INITIALIZER(TEST_FEW));
		}

		TEST_METHOD(TestLayerdNeuralNetworkUse)
		{
			Allocator* oAllocator = Allocator::GetDefault();
			
			LayerdNeuralNetwork* oNetwork = new LayerdNeuralNetwork(TEST_INITIALIZER(TEST_FEW));
			DataTypesLayerd::DInput* lInputs = oAllocator->Allocate<DataTypesLayerd::DInput>(TEST_FEW);
			DataTypesLayerd::DOutput* lOutputs = oAllocator->Allocate<DataTypesLayerd::DOutput>(TEST_FEW);
			DataTypesLayerd::DOutput* lExpectedOutput = oAllocator->Allocate<DataTypesLayerd::DOutput>(TEST_FEW);
			Fx_InitializeInput(lInputs, TEST_FEW, TEST_INIT_INPUT);
			
			oNetwork->Use(lInputs, lOutputs);
			
			delete oNetwork;

			oAllocator->DeAllocate(lInputs);
			oAllocator->DeAllocate(lOutputs);
		}

		TEST_METHOD(TestLayerdNeuralNetworkTrain)
		{
			// TODO: Your test code here
		}

		TEST_METHOD(TestLayerdNeuralNetworkValidate)
		{
			// TODO: Your test code here
		}

		TEST_METHOD(TestLayerdNeuralNetworkOptimize)
		{
			// TODO: Your test code here
		}
	};
}