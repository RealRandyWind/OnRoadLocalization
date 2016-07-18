#ifndef DATAVECTOR_HPP
#define DATAVECTOR_HPP

#include "Allocator.hpp"

template<typename Element = float>
class DataVector
{
	friend class DataVector;
private:
	Element* a_ptrData;
	unsigned int a_nN;
	unsigned int a_nM;
	unsigned int a_eFlag;
	unsigned int a_nData;
	Allocator* a_oAllocator;
	bool a_bIsPlaceholder;

	DataVector* Fx_Resize(unsigned int nN, unsigned int nM)
	{
		a_nN = nN; a_nM = nM;
		unsigned int nSize = nN*nM;
		if (nSize > a_nData)
		{
			a_ptrData = a_oAllocator->ReAllocate<Element>(a_ptrData, nSize);
			a_nData = nSize;
		}
		return this;
	}
public:
	DataVector(unsigned int nN, unsigned int nM = 1, Allocator* oCustomAllocator = Allocator::GetDefault(), bool bIsPlaceholder = false)
	{
		a_oAllocator = oCustomAllocator;
		a_nData = nN * nM;
		a_ptrData = bIsPlaceholder ? nullptr : a_oAllocator->Allocate<Element>(a_nData);
		a_nN = nN;
		a_nM = nM;
		a_eFlag = 0;
		a_bIsPlaceholder = bIsPlaceholder;
	}

	~DataVector() { if (!a_bIsPlaceholder) a_oAllocator->DeAllocate(a_ptrData);  }

	/* Operator Add function wrappers */
	template<typename T, typename R = Element>
	DataVector<R>* Add(DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		return Add2<T, R>(this, oVectorB, oResult);
	}

	DataVector* Add(DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Add2(this, oVectorB, oResult);
	}

	static DataVector* Add2(DataVector* oVectorA, DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Add2<Element>(oVectorA, oVectorB, oResult);
	}

	/* Operator Sub function wrappers */
	template<typename T, typename R = Element>
	DataVector<R>* Sub(DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		return Sub2<T, R>(this, oVectorB, oResult);
	}

	DataVector* Sub(DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Sub2(this, oVectorB, oResult);
	}

	static DataVector* Sub2(DataVector* oVectorA, DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Sub2<Element>(oVectorA, oVectorB, oResult);
	}

	/* Operator Mul function wrappers */
	template<typename T, typename R = Element>
	DataVector<R>* Mul(DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		return Mul2<T, R>(this, oVectorB, oResult);
	}

	DataVector* Mul(DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Mul2(this, oVectorB, oResult);
	}

	static DataVector* Mul2(DataVector* oVectorA, DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Mul2<Element>(oVectorA, oVectorB, oResult);
	}

	/* Operator EMul function wrappers */
	template<typename T, typename R = Element>
	DataVector<R>* EMul(DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		return EMul2<T, R>(this, oVectorB, oResult);
	}

	DataVector* EMul(DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return EMul2(this, oVectorB, oResult);
	}

	static DataVector* EMul2(DataVector* oVectorA, DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return EMul2<Element>(oVectorA, oVectorB, oResult);
	}

	/* Operator Assign function wrappers */
	template<typename T, typename R = Element>
	DataVector<R>* Assign(DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		return Assign2<T, R>(this, oVectorB, oResult);
	}

	DataVector* Assign(DataVector* oVectorB, DataVector* oResult = nullptr)
	{
		return Assign2(this, oVectorB, oResult);
	}

	/* Operator Definitions */
	template<typename T, typename R>
	static DataVector<R>* Mul2(DataVector* oVectorA, DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		if (!oVectorA || !oVectorB || !oVectorA->Size() || !oVectorB->Size() 
			|| oVectorB->a_nM != oVectorA->a_nN) { return oResult; }

		unsigned int nM = oVectorA->a_nM;
		unsigned int nN = oVectorB->a_nN;
		unsigned int nK = oVectorA->a_nN;

		oResult = oResult ? oResult->Fx_Resize(nN, nM) : new DataVector<R>(nN, nM, oVectorA->a_oAllocator, false);

		unsigned int iIndex = oResult->Size();
		while (iIndex--)
		{
			unsigned int iM = iIndex / nN;
			unsigned int iN = iIndex % nN;

			oResult->a_ptrData[iIndex] = oVectorA->a_ptrData[iM * nK] * oVectorB->a_ptrData[iN];
			for (unsigned int iK = 1; iK < nK; ++iK)
			{
				oResult->a_ptrData[iIndex] = oResult->a_ptrData[iIndex] + oVectorA->a_ptrData[iM * nK + iK] * oVectorB->a_ptrData[iK * nN + iN];
			}
		}
		return oResult;
	}

	template<typename T, typename R>
	static DataVector<R>* Sub2(DataVector* oVectorA, DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		if (!oVectorA || !oVectorB || !oVectorA->Size() || !oVectorB->Size() 
			|| oVectorB->a_nN != oVectorA->a_nN || oVectorB->a_nM != oVectorA->a_nM) { return oResult; }

		oResult = oResult ? oResult->Fx_Resize(oVectorA->a_nN, oVectorA->a_nM) : oVectorA;

		unsigned int iIndex = oResult->Size();
		while (iIndex--)
		{
			oResult->a_ptrData[iIndex] = oVectorA->a_ptrData[iIndex] - oVectorB->a_ptrData[iIndex];
		}
		return oResult;
	}

	template<typename T, typename R>
	static DataVector<R>* Add2(DataVector* oVectorA, DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		if (!oVectorA || !oVectorB || !oVectorA->Size() || !oVectorB->Size()
			|| oVectorB->a_nN != oVectorA->a_nN || oVectorB->a_nM != oVectorA->a_nM) { return oResult; }

		oResult = oResult ? oResult->Fx_Resize(oVectorA->a_nN, oVectorA->a_nM) : oVectorA;

		unsigned int iIndex = oResult->Size();
		while (iIndex--)
		{
			oResult->a_ptrData[iIndex] = oVectorA->a_ptrData[iIndex] + oVectorB->a_ptrData[iIndex];
		}
		return oResult;
	}

	template<typename T, typename R>
	static DataVector<R>* EMul2(DataVector* oVectorA, DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		if (!oVectorA || !oVectorB || !oVectorA->Size() || !oVectorB->Size()
			|| oVectorB->a_nN != oVectorA->a_nN || oVectorB->a_nM != oVectorA->a_nM) { return oResult; }

		oResult = oResult ? oResult->Fx_Resize(oVectorA->a_nN, oVectorA->a_nM) : oVectorA;

		unsigned int iIndex = oResult->Size();
		while (iIndex--)
		{
			oResult->a_ptrData[iIndex] = oVectorA->a_ptrData[iIndex] * oVectorB->a_ptrData[iIndex];
		}
		return oResult;
	}

	template<typename T, typename R>
	static DataVector<R>* Assign2(DataVector* oVectorA, DataVector<T>* oVectorB, DataVector<R>* oResult = nullptr)
	{
		if (!oVectorA || !oVectorB || !oVectorA->Size() || !oVectorB->Size()
			|| oVectorB->a_nN != oVectorA->a_nN || oVectorB->a_nM != oVectorA->a_nM) {
			return oResult;
		}

		oResult = oResult ? oResult->Fx_Resize(oVectorA->a_nN, oVectorA->a_nM) : oVectorA;

		unsigned int iIndex = oResult->Size();
		while (iIndex--)
		{
			oResult->a_ptrData[iIndex] = oVectorB->a_ptrData[iIndex];
		}
		return oResult;
	}

	/* Special Functions */
	Element* Place(Element* ptrData, unsigned int nData = 0)
	{
		Element* ptrOldData = a_ptrData;
		a_ptrData = ptrData;
		a_nData = nData ? nData : a_nData;
		return ptrOldData;
	}

	Element* Data() { return a_ptrData; }
	unsigned int Size() { return a_nN * a_nM; }
	unsigned int GetErrorCode() { return a_eErrorCode; }

	static DataVector* Initialize(DataVector* oDataVector, Element oValue)
	{
		unsigned int iIndex = oDataVector->Size();
		while (iIndex--) { oDataVector->a_ptrData[iIndex] = oValue; }
		return oDataVector;
	}
};
#endif
