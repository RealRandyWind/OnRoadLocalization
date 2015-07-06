#ifndef MOPENCLMANAGER_HPP
#define MOPENCLMANAGER_HPP
#include "CL/opencl.h"
#include "OpenCLProgram.hpp"
#include "OpenCLArgument.hpp"
#include "MLogManager.hpp"
#include <vector>
#include <string>

enum EOpenCLMemType
{
	OPENCL_MEM_READ_WRITE = CL_MEM_READ_WRITE,
	OPENCL_MEM_WRITE_ONLY = CL_MEM_WRITE_ONLY,
	OPENCL_MEM_READ_ONLY = CL_MEM_READ_ONLY,
	OPENCL_MEM_USE_HOST_PTR = CL_MEM_USE_HOST_PTR,
	OPENCL_MEM_ALLOC_HOST_PTR = CL_MEM_ALLOC_HOST_PTR,
	OPENCL_MEM_COPY_HOST_PTR = CL_MEM_COPY_HOST_PTR,

	OPENCL_MEM_DEFAULT = OPENCL_MEM_READ_WRITE | OPENCL_MEM_COPY_HOST_PTR
};

class MOpenCLManager;
class MOpenCLManager
{
private:
	MOpenCLManager();
	static MOpenCLManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	OpenCLProgram* a_oActiveProgram;
	unsigned int a_iActiveProgram;

	MLogManager* a_mLogManager;

	cl_platform_id a_clPlatform;
	cl_device_id a_clDevice;
	cl_context a_clContext;
	cl_command_queue a_clCommandQueue;
	cl_ulong a_clMaxMemSize;
	cl_event a_clDone;

	std::vector<OpenCLProgram*> a_lPrograms;

	bool a_bIsRunning;
	
	cl_int a_clError;

	unsigned int Fx_CompileProgram(std::string sSource, std::string sFunction, unsigned int iArgumentCount);
	void Fx_DestroyProgram(OpenCLProgram* oProgram);
	void Fx_LoadArguments();
	void Fx_DestroyArgument(OpenCLArgument* oArgument);
	void Fx_EnqueueTask();
	static char* Fx_MapErrorCode(unsigned int iErrorCode);
public:
	~MOpenCLManager();
	static MOpenCLManager* GetInstance();

	void SetUp();
	void ShutDown();

	unsigned int LoadProgramFromFile(std::string sPath, unsigned int iArgumentCount);
	unsigned int LoadProgramFromFile(std::string sPath, std::string sFunction, unsigned int iArgumentCount);
	unsigned int LoadProgramFromString(std::string sSource, std::string sFunction, unsigned int iArgumentCount);

	void SetArgument(unsigned int iArgument, void* ptrValue, unsigned int iSize, bool bIsRaw=false, bool bIsNull=false, unsigned int eType=OPENCL_MEM_DEFAULT);
	void SetChuncksTo(unsigned int iDimensions, size_t* aiOffset, size_t* aiGlobalRange, size_t* aiLocalRange);

	void Use(unsigned int iProgram);
	void UnUse();
	void Destroy(unsigned int iProgram=0);
	void Finish();
	void Compute();
	void Result(unsigned int iArgument, void* ptrValue, bool bIsWaitForCompleteResults=true);

	unsigned int GetActiveProgram();

};
#endif
