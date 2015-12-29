#include "OpenCLProgram.hpp"

OpenCLProgram::OpenCLProgram(unsigned int iProgram, cl_program clProgram, cl_kernel clKernel, std::string sFunction, unsigned int iArgumentCount, std::string sOptions)
{
	a_iProgram = iProgram;
	a_iArgumentCount = iArgumentCount;
	a_iDimensions = 1;
	a_clProgram = clProgram;
	a_clKernel = clKernel;
	a_sFunction = sFunction;
	a_sOptions = sOptions;

	a_lArguments.resize(a_iArgumentCount,NULL);
	a_iNumberOfArgumentsSet = 0;

	a_aiOffset = NULL;
	a_aiGlobalRange = NULL;
	a_aiLocalRange = NULL;
}

OpenCLProgram::~OpenCLProgram()
{

}

bool OpenCLProgram::IsParallel()
{
	return !a_aiGlobalRange || a_iDimensions <= 0 || !a_aiOffset || !a_aiLocalRange;
}
