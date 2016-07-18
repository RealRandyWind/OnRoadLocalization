#include "stdint.h"

namespace DataTypesLayerd
{
	#define MAX_CHARGE 4*64
	#define MAX_COLOR_VALUE 255
	struct DInput;
	struct DSynapse;
	struct DNeuron;
	struct DOutput;
	struct DCharge;

	
	/* Data Structure Elements */
	typedef DSynapse DInputSynapse;
	typedef DSynapse DOutputSynapse;

	struct DCharge
	{
		float dChargeR, dChargeG, dChargeB;

		friend DCharge& operator+(DCharge& oLhs, const DCharge& oRhs);
	};

	struct DSynapse
	{
		float dWeightR, dWeightG, dWeightB;

		friend DCharge& operator*(DSynapse& oLhs, const DCharge& oRhs);
	};

	struct DNeuron
	{
		float dBias;

		friend DCharge& operator+(DCharge& oLhs, const DNeuron& oRhs);
	};

	struct DInput
	{
		uint8_t nR, nG, nB, nA;

		friend DCharge& operator*(DSynapse& oLhs, const DInput& oRhs);
	};

	struct DOutput
	{
		float dProbability;

		friend DOutput& operator+(DOutput& oLhs, const DCharge& oRhs);
		DOutput& operator=(DCharge& oRhs);
	};

	/* Argument Parameters */

	struct DValidation
	{
	};

	struct DTraining
	{
		float dLearningRate;
		float dMinimumError;
		float dMaximumEpochs;
	};

	struct DOptimization
	{
	};

	struct DObjectInitializer
	{
		unsigned int nInput;
		unsigned int nOutput;
		unsigned int nHidden;
		unsigned int nLayers;
		unsigned int nBytesState;
		void* ptrState;
	};
}