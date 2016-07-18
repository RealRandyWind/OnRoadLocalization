#include "LayerdNeuralNetwork.datatypes.hpp"

namespace DataTypesLayerd
{
	/* C × C -> C */
	DCharge& operator+(DCharge& oLhs, const DCharge& oRhs)
	{
		oLhs.dChargeR += oRhs.dChargeR;
		oLhs.dChargeG += oRhs.dChargeG;
		oLhs.dChargeB += oRhs.dChargeB;
		return oLhs;
	}

	/* S × C -> C */
	DCharge& operator*(DSynapse& oLhs, const DCharge& oRhs)
	{
		DCharge oReturn;
		oReturn.dChargeR = oLhs.dWeightR * oRhs.dChargeR;
		oReturn.dChargeG = oLhs.dWeightG * oRhs.dChargeG;
		oReturn.dChargeB = oLhs.dWeightB * oRhs.dChargeB;
		return oReturn;
	}

	/* C + N -> C */
	DCharge& operator+(DCharge& oLhs, const DNeuron& oRhs)
	{
		oLhs.dChargeR += oRhs.dBias;
		oLhs.dChargeG += oRhs.dBias;
		oLhs.dChargeB += oRhs.dBias;
		return oLhs;
	}

	/* S × I -> C */
	DCharge& operator*(DSynapse& oLhs, const DInput& oRhs)
	{
		DCharge oReturn;
		float dAlpha = oRhs.nA / MAX_COLOR_VALUE;
		oReturn.dChargeR = oLhs.dWeightR * (oRhs.nR / MAX_COLOR_VALUE) * dAlpha;
		oReturn.dChargeG = oLhs.dWeightG * (oRhs.nG / MAX_COLOR_VALUE) * dAlpha;
		oReturn.dChargeB = oLhs.dWeightB * (oRhs.nB / MAX_COLOR_VALUE) * dAlpha;
		return oReturn;
	}

	/* O + C -> O */
	DOutput& operator+(DOutput& oLhs, const DCharge& oRhs)
	{
		oLhs.dProbability += oRhs.dChargeR
			+ oRhs.dChargeG
			+ oRhs.dChargeB;
		return oLhs;
	}

	/* C -> O */
	DOutput& DOutput::operator=(DCharge& oRhs)
	{
		dProbability = oRhs.dChargeR
			+ oRhs.dChargeG
			+ oRhs.dChargeB;
		return *this;
	}
}