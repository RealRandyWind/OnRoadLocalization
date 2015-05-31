#include "OpenCLArgument.hpp"

OpenCLArgument::OpenCLArgument(unsigned int iArgument, cl_mem clValue, size_t iSize, bool bIsRaw)
{
	a_iArgument = iArgument;
	a_clValue = clValue;
	a_iSize = iSize;
	a_bIsRaw = bIsRaw;
}

OpenCLArgument::OpenCLArgument(unsigned int iArgument, void* ptrValue, size_t iSize, bool bIsRaw)
{
	a_iArgument = iArgument;
	a_ptrValue = ptrValue;
	a_iSize = iSize;
	a_bIsRaw = bIsRaw;
}

OpenCLArgument::~OpenCLArgument()
{

}
