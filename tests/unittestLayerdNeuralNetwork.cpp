#include "stdafx.h"
#include "CppUnitTest.h"
#include "LayerdNeuralNetwork.hpp"
#include "Allocator.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	#define TEST_ONE 1
	#define TEST_TWO 2
	#define TEST_THREE 3
	#define TEST_FEW 16
	#define TEST_INITIALIZER(N) {(N),(N),2 * (N),(N), 0, nullptr}
	#define TEST_INIT_INPUT { 255, 255, 255, 255 }
	#define TEST_INIT_HELPER_ALPHA(N) ((N+1) * 3)
	#define TEST_INIT_SYNAPSE(N) { 1.0/TEST_INIT_HELPER_ALPHA((N)), 1.0/TEST_INIT_HELPER_ALPHA((N)), 1.0/TEST_INIT_HELPER_ALPHA((N))) }
	#define TEST_INIT_NEURON(N) { 1.0/TEST_INIT_HELPER_ALPHA((N)) }
	#define TEST_INIT_OUTPUT { 1.0 }

	TEST_CLASS(TestLayerdNeuralNetwork)
	{
	public:
		DataTypesLayerd::DInput* Fx_InitializeInput(DataTypesLayerd::DInput* lInputs, unsigned int nInputs, DataTypesLayerd::DInput oValue)
		{
			unsigned int iIndex = nInputs;
			while (iIndex--) { lInputs[iIndex] = oValue; }
			return lInputs;
		}

		DataTypesLayerd::DOutput* Fx_InitializeInput(DataTypesLayerd::DOutput* lOutputs, unsigned int nInputs, DataTypesLayerd::DOutput oValue)
		{
			unsigned int iIndex = nInputs;
			while (iIndex--) { lOutputs[iIndex] = oValue; }
			return lOutputs;
		}

		void* Fx_InitializeState(void* ptrState, DataTypesLayerd::DObjectInitializer)
		{

			return ptrState;
		}

		DataTypesLayerd::DOutput* Fx_InitialzeExpectedOutput(DataTypesLayerd::DOutput* lOutputs, unsigned int nOutputs)
		{
			unsigned int iIndex = nOutputs;
			while (iIndex) {
				lOutputs[iIndex] = {}
			}
			return lOutputs;
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