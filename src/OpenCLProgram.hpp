#ifndef OPENCLPROGRAM_HPP
#define OPENCLPROGRAM_HPP
#include "CL/opencl.h"
#include "OpenCLArgument.hpp"
#include <string>
#include <vector>

class OpenCLProgram;
class OpenCLProgram
{
public:
	unsigned int a_iProgram;
	unsigned int a_iArgumentCount;
	unsigned int a_iNumberOfArgumentsSet;
	unsigned int a_iDimensions;
	cl_program a_clProgram;
	cl_kernel a_clKernel;

	std::string a_sFunction;
	std::string a_sOptions;

	std::vector<OpenCLArgument*> a_lArguments;

	size_t* a_aiOffset;
	size_t* a_aiGlobalRange; 
	size_t* a_aiLocalRange;
	
	OpenCLProgram(unsigned int iProgram, cl_program clProgram, cl_kernel clKernel, std::string sFunction, unsigned int iArgumentCount, std::string sOptions);
	~OpenCLProgram();
	bool IsParallel();
};
#endif
