#ifndef OPENCLARGUMENT_HPP
#define OPENCLARGUMENT_HPP
#include "CL/opencl.h"

class OpenCLArgument;
class OpenCLArgument
{
public:
	unsigned int a_iArgument;
	size_t a_iSize;
	union {
		cl_mem a_clValue;
		void* a_ptrValue;
	};
	bool a_bIsRaw;
	OpenCLArgument(unsigned int iArgument, cl_mem clValue, size_t iSize, bool bIsRaw);
	OpenCLArgument(unsigned int iArgument, void* ptrValue, size_t iSize, bool bIsRaw);
	~OpenCLArgument();
};
#endif
