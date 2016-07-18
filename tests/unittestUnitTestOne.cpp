#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{		
	TEST_CLASS(TestUnitTestOne)
	{
	public:
		
		TEST_METHOD(TestUnitTestOneMethod)
		{
			Assert::AreEqual(0, 0);
		}

	};
}