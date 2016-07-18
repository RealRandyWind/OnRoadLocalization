#include "stdafx.h"
#include "CppUnitTest.h"
#include "DataVector.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
	struct TestType2;
	struct TestType;

	struct TestType
	{
		float a, b;
		TestType() { a = 0; b = 1; }
		TestType(float x, float y) { a = x; b = y; }

		friend TestType& operator+(TestType& oTypeA, const TestType& oTypeB) { oTypeA.a += oTypeB.a; oTypeA.b += oTypeB.b; return oTypeA; }
		friend TestType& operator-(TestType& oTypeA, const TestType& oTypeB) { oTypeA.a -= oTypeB.a; oTypeA.b -= oTypeB.b; return oTypeA; }
		friend TestType& operator*(TestType& oTypeA, const TestType& oTypeB) { oTypeA.a *= oTypeB.a; oTypeA.b *= oTypeB.b; return oTypeA; }

		friend TestType& operator+(TestType& oTypeA, const TestType2& oTypeB);
		friend TestType& operator-(TestType& oTypeA, const TestType2& oTypeB);
		friend TestType& operator*(TestType& oTypeA, const TestType2& oTypeB);
		TestType& operator=(const TestType2& oTypeB);

		/* for testing only */
		friend bool operator==(const TestType& oTypeA, const TestType& oTypeB) { return oTypeA.a == oTypeB.a && oTypeA.b == oTypeB.b; }
	};

	struct TestType2
	{
		float c, d;
		TestType2() { c = 0; d = 1; }
		TestType2(float x, float y) { c = x; d = y; }

		friend TestType2& operator+(TestType2& oTypeA, const TestType2& oTypeB) { oTypeA.c += oTypeB.c; oTypeA.d += oTypeB.d; return oTypeA; }
		friend TestType2& operator-(TestType2& oTypeA, const TestType2& oTypeB) { oTypeA.c -= oTypeB.c; oTypeA.d -= oTypeB.d; return oTypeA; }
		friend TestType2& operator*(TestType2& oTypeA, const TestType2& oTypeB) { oTypeA.c *= oTypeB.c; oTypeA.d *= oTypeB.d; return oTypeA; }

		friend TestType2& operator+(TestType2& oTypeA, const TestType& oTypeB);
		friend TestType2& operator-(TestType2& oTypeA, const TestType& oTypeB);
		friend TestType2& operator*(TestType2& oTypeA, const TestType& oTypeB);
		TestType2& operator=(const TestType& oTypeB);

		/* for testing only */
		friend bool operator==(const TestType2& oTypeA, const TestType2& oTypeB) { return oTypeA.c == oTypeB.c && oTypeA.d == oTypeB.d; }
	};

	TestType& tests::operator+(TestType& oTypeA, const TestType2& oTypeB) { oTypeA.a += oTypeB.c; oTypeA.b += oTypeB.d; return oTypeA; }
	TestType& tests::operator-(TestType& oTypeA, const TestType2& oTypeB) { oTypeA.a -= oTypeB.c; oTypeA.b -= oTypeB.d; return oTypeA; }
	TestType& tests::operator*(TestType& oTypeA, const TestType2& oTypeB) { oTypeA.a *= oTypeB.c; oTypeA.b *= oTypeB.d; return oTypeA; }
	TestType& TestType::operator=(const TestType2& oTypeB) { a = oTypeB.c; b = oTypeB.d; return *this; }

	TestType2& tests::operator+(TestType2& oTypeA, const TestType& oTypeB) { oTypeA.c += oTypeB.a; oTypeA.d += oTypeB.b; return oTypeA; }
	TestType2& tests::operator-(TestType2& oTypeA, const TestType& oTypeB) { oTypeA.c -= oTypeB.a; oTypeA.d -= oTypeB.b; return oTypeA; }
	TestType2& tests::operator*(TestType2& oTypeA, const TestType& oTypeB) { oTypeA.c *= oTypeB.a; oTypeA.d *= oTypeB.b; return oTypeA; }
	TestType2& TestType2::operator=(const TestType& oTypeB) { c *= oTypeB.a; d *= oTypeB.b; return *this; }

	#define TEST_TYPE TestType
	#define TEST_TYPE2 TestType2
	#define TEST_ZERO 0
	#define TEST_ONE 1
	#define TEST_TWO 2
	#define TEST_THREE 3
	#define TEST_SOME_LAYERS 32
	#define TEST_SOME 480 * 272
	#define TEST_MANY_LAYERS 1024
	#define TEST_MANY 4096 * 1716
	#define TEST_INIT {0, 1}
	#define TEST_EXPECTED_ADD {0, 2}
	#define TEST_EXPECTED_EMUL {0, 1}
	#define TEST_EXPECTED_SUB {0, 0}
	#define TEST_EXPECTED_MUL(V) {0, (V)}

	TEST_CLASS(TestDataVector)
	{
	public:
		template<typename T>
		void Fx_TestDataVector(DataVector<T>* oDataVector, unsigned int nExpectedSize, T oExpectedType)
		{
			Assert::IsTrue(oDataVector->Size() == nExpectedSize, L"Fx_TestDataVector::DataVector ExpectedSize");

			TEST_TYPE* lData = oDataVector->Data();
			unsigned int iIndex = nExpectedSize;
			while (iIndex--)
			{
				Assert::IsTrue(lData[iIndex] == oExpectedType, L"Fx_TestDataVector::DataVector ExpectedElemenet");
				lData[iIndex] = oExpectedType;
			}
		}

		void Fx_TestDataVector(DataVector<TestType>* oDataVector, unsigned int nExpectedSize, TestType oExpectedType)
		{
			Fx_TestDataVector<TestType>(oDataVector, nExpectedSize, oExpectedType);
		}

		TEST_METHOD(TestDataVectorTestType)
		{
			Assert::IsTrue(sizeof(TestType) == sizeof(float) * 2);
			
			TestType oCorrectType = TEST_INIT;

			TestType* ptrType = (TestType*)Allocator::GetDefault()->Allocate(sizeof(TestType));

			TestType oType;
			Assert::IsTrue(oType == oCorrectType);
			Assert::IsFalse(*ptrType == oCorrectType);
		}

		TEST_METHOD(TestDataVectorConstructor)
		{
			DataVector<>* oDataVector;
			DataVector<TEST_TYPE>* oDataVector2;

			/* test base zero */
			oDataVector = new DataVector<>(TEST_ZERO);
			oDataVector2 = new DataVector<TEST_TYPE>(TEST_ZERO);
			delete oDataVector;
			delete oDataVector2;

			/* test base one */
			oDataVector = new DataVector<>(TEST_ONE);
			oDataVector2 = new DataVector<TEST_TYPE>(TEST_ONE);
			delete oDataVector;
			delete oDataVector2;

			/* test base two */
			oDataVector = new DataVector<>(TEST_TWO);
			oDataVector2 = new DataVector<TEST_TYPE>(TEST_TWO);
			delete oDataVector;
			delete oDataVector2;

			/* test base three */
			oDataVector = new DataVector<>(TEST_THREE);
			oDataVector2 = new DataVector<TEST_TYPE>(TEST_THREE);
			delete oDataVector;
			delete oDataVector2;
		}

		TEST_METHOD(TestDataVectorOperations)
		{
			DataVector<TEST_TYPE>* oDataVector;
			DataVector<TEST_TYPE>* oDataVector2;
			DataVector<TEST_TYPE>* oDataVector3;
			DataVector<TEST_TYPE2>* oDataVector4;
			DataVector<TEST_TYPE>* oDataMatrix;
			DataVector<TEST_TYPE>* oDataMatrix1;

			/* test base zero */
			oDataVector = new DataVector<TEST_TYPE>(TEST_ZERO);
			oDataVector2 = new DataVector<TEST_TYPE>(TEST_ZERO);
			oDataVector3 = new DataVector<TEST_TYPE>(1, TEST_ZERO);
			oDataVector4 = new DataVector<TEST_TYPE2>(TEST_ZERO);
			oDataMatrix = new DataVector<TEST_TYPE>(TEST_ZERO, TEST_ZERO);
			Assert::IsNull(oDataVector->Add(oDataVector2));
			Assert::IsNull(oDataVector->EMul(oDataVector2));
			Assert::IsNull(oDataVector->Sub(oDataVector2));
			Assert::IsNull(oDataMatrix1 = oDataVector->Mul(oDataVector3));
			Assert::IsNull(oDataMatrix1 = oDataVector3->Mul(oDataVector));
			Assert::IsNull(oDataMatrix1 = oDataVector->Mul(oDataMatrix));
			Assert::IsNull(oDataVector2->Add<TEST_TYPE2>(oDataVector4));
			delete oDataVector;
			delete oDataVector2;
			delete oDataVector3;
			delete oDataVector4;
			delete oDataMatrix;

			/* test base one */
			oDataVector = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_ONE), TEST_INIT);
			oDataVector2 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_ONE), TEST_INIT);
			oDataVector3 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(1,TEST_ONE), TEST_INIT);
			oDataVector4 = DataVector<TEST_TYPE2>::Initialize(new DataVector<TEST_TYPE2>(TEST_ONE), TEST_INIT);
			oDataMatrix = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_ONE, TEST_ONE), TEST_INIT);
			Fx_TestDataVector(oDataVector->Add(oDataVector2), TEST_ONE, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector->Assign(oDataVector2), TEST_ONE, TEST_INIT);
			Fx_TestDataVector(oDataVector->EMul(oDataVector2), TEST_ONE, TEST_EXPECTED_EMUL);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataVector->Sub(oDataVector2), TEST_ONE, TEST_EXPECTED_SUB);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataVector3, new DataVector<TEST_TYPE>(1)), 1, TEST_EXPECTED_MUL(TEST_ONE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector3->Mul(oDataVector, new DataVector<TEST_TYPE>(TEST_ONE, TEST_ONE)), TEST_ONE, TEST_EXPECTED_EMUL);
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_ONE, TEST_ONE)), TEST_ONE, TEST_EXPECTED_MUL(TEST_ONE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_ONE)), TEST_ONE, TEST_EXPECTED_MUL(TEST_ONE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataVector3, new DataVector<TEST_TYPE>(1, TEST_ONE)), TEST_ONE, TEST_EXPECTED_MUL(TEST_ONE));
			Fx_TestDataVector(oDataVector2->Add<TEST_TYPE2>(oDataVector4), TEST_ONE, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector2->Assign<TEST_TYPE2>(oDataVector4), TEST_ONE, TEST_INIT);
			delete oDataVector;
			delete oDataVector2;
			delete oDataVector3;
			delete oDataVector4;
			delete oDataMatrix;
			delete oDataMatrix1;

			/* test base two */
			oDataVector = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_TWO), TEST_INIT);
			oDataVector2 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_TWO), TEST_INIT);
			oDataVector3 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(1, TEST_TWO), TEST_INIT);
			oDataVector4 = DataVector<TEST_TYPE2>::Initialize(new DataVector<TEST_TYPE2>(TEST_TWO), TEST_INIT);
			oDataMatrix = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_TWO, TEST_TWO), TEST_INIT);
			Fx_TestDataVector(oDataVector->Add(oDataVector2), TEST_TWO, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector->Assign(oDataVector2), TEST_TWO, TEST_INIT);
			Fx_TestDataVector(oDataVector->EMul(oDataVector2), TEST_TWO, TEST_EXPECTED_EMUL);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataVector->Sub(oDataVector2), TEST_TWO, TEST_EXPECTED_SUB);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataVector3, new DataVector<TEST_TYPE>(1)), 1, TEST_EXPECTED_MUL(TEST_TWO));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector3->Mul(oDataVector, new DataVector<TEST_TYPE>(TEST_TWO, TEST_TWO)), TEST_TWO * TEST_TWO, TEST_EXPECTED_EMUL);
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_TWO, TEST_TWO)), TEST_TWO * TEST_TWO, TEST_EXPECTED_MUL(TEST_TWO));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_TWO)), TEST_TWO, TEST_EXPECTED_MUL(TEST_TWO));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataVector3, new DataVector<TEST_TYPE>(1, TEST_TWO)), TEST_TWO, TEST_EXPECTED_MUL(TEST_TWO));
			Fx_TestDataVector(oDataVector2->Add<TEST_TYPE2>(oDataVector4), TEST_TWO, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector2->Assign<TEST_TYPE2>(oDataVector4), TEST_TWO, TEST_INIT);
			delete oDataVector;
			delete oDataVector2;
			delete oDataVector3;
			delete oDataVector4;
			delete oDataMatrix;
			delete oDataMatrix1;

			/* test base three */
			oDataVector = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_THREE), TEST_INIT);
			oDataVector2 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_THREE), TEST_INIT);
			oDataVector3 = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(1, TEST_THREE), TEST_INIT);
			oDataVector4 = DataVector<TEST_TYPE2>::Initialize(new DataVector<TEST_TYPE2>(TEST_THREE), TEST_INIT);
			oDataMatrix = DataVector<TEST_TYPE>::Initialize(new DataVector<TEST_TYPE>(TEST_THREE, TEST_THREE), TEST_INIT);
			Fx_TestDataVector(oDataVector->Add(oDataVector2), TEST_THREE, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector->Assign(oDataVector2), TEST_THREE, TEST_INIT);
			Fx_TestDataVector(oDataVector->EMul(oDataVector2), TEST_THREE, TEST_EXPECTED_EMUL);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataVector->Sub(oDataVector2), TEST_THREE, TEST_EXPECTED_SUB);
			DataVector<TEST_TYPE>::Initialize(oDataVector, TEST_INIT);
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataVector3, new DataVector<TEST_TYPE>(1)), 1, TEST_EXPECTED_MUL(TEST_THREE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector3->Mul(oDataVector, new DataVector<TEST_TYPE>(TEST_THREE, TEST_THREE)), TEST_THREE * TEST_THREE, TEST_EXPECTED_EMUL);
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_THREE, TEST_THREE)), TEST_THREE * TEST_THREE, TEST_EXPECTED_MUL(TEST_THREE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataVector->Mul(oDataMatrix, new DataVector<TEST_TYPE>(TEST_THREE)), TEST_THREE, TEST_EXPECTED_MUL(TEST_THREE));
			delete oDataMatrix1;
			Fx_TestDataVector(oDataMatrix1 = oDataMatrix->Mul(oDataVector3, new DataVector<TEST_TYPE>(1, TEST_THREE)), TEST_THREE, TEST_EXPECTED_MUL(TEST_THREE));
			Fx_TestDataVector(oDataVector2->Add<TEST_TYPE2>(oDataVector4), TEST_THREE, TEST_EXPECTED_ADD);
			Fx_TestDataVector(oDataVector2->Assign<TEST_TYPE2>(oDataVector4), TEST_THREE, TEST_INIT);
			delete oDataVector;
			delete oDataVector2;
			delete oDataVector3;
			delete oDataVector4;
			delete oDataMatrix;
			delete oDataMatrix1;
		}
	};
}