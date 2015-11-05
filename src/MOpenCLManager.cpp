#include "MOpenCLManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#define MOPENCL_READLINE_SIZE 256
#define MOPENCL_FUNCTIONNAME_SIZE 64

MOpenCLManager* MOpenCLManager::a_oInstance = 0;

MOpenCLManager::MOpenCLManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;
	a_oActiveProgram = NULL;
	a_iActiveProgram = 0;

	a_mLogManager = MLogManager::GetInstance();

	a_clPlatform = 0;
	a_clDevice = 0;
	a_clError = CL_SUCCESS;
	a_clMaxMemSize = 0;

	a_bIsRunning = false;

	a_lPrograms.push_back(NULL);
}

MOpenCLManager::~MOpenCLManager()
{

}

MOpenCLManager* MOpenCLManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MOpenCLManager();
	return a_oInstance;
}

void MOpenCLManager::SetUp()
{
	if(a_bIsSetUp) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetUp] already setup.");
		return; 
	}
	a_bIsShutDown = false;

	
	a_clError = clGetPlatformIDs(1,&a_clPlatform,NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.SetUp] unable to find any platform.");
		return;
	}

	a_clError = clGetDeviceIDs(a_clPlatform,CL_DEVICE_TYPE_GPU,1,&a_clDevice,NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.SetUp] unable to find any device.");
		return;
	}

	a_clContext = clCreateContext(NULL, 1, &a_clDevice, NULL, NULL, &a_clError);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.SetUp] unable to create context for device.");
		return;
	}

	a_clCommandQueue = clCreateCommandQueue(a_clContext, a_clDevice, 0, NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.SetUp] unable to create command queue for device.");
		return;
	}

	
	a_clError = clGetDeviceInfo(a_clDevice, CL_DEVICE_MAX_MEM_ALLOC_SIZE , sizeof(cl_ulong), &a_clMaxMemSize, 0);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetUp] unable to retrieve maximum memory size available by current device.");
	}
	a_mLogManager->Info(0,"[MOpenCLManager.SetUp] device has %lu of memmory available.",a_clMaxMemSize);


	a_bIsSetUp = true;
	a_mLogManager->Success(0, "[MOpenCLManager.SetUp] done.");
}

void MOpenCLManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0, "[MOpenCLManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;

	for (OpenCLProgram* oProgram : a_lPrograms) { Fx_DestroyProgram(oProgram); }

	a_clError = clReleaseContext(a_clContext);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.ShuDown] unable to release context for device.");
		return;
	}
	a_clContext = 0;

	a_clError = clReleaseCommandQueue(a_clCommandQueue);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.ShutDown] unable to release command queue for device.");
		return;
	}
	a_clCommandQueue = 0;

	a_clDevice = 0;
	a_clPlatform = 0;
	a_clMaxMemSize = 0;

	a_bIsShutDown = true;
	a_mLogManager->Success(0, "[MOpenCLManager.ShutDown] done.");
}

void MOpenCLManager::Fx_EnqueueTask()
{
	if(!a_oActiveProgram->IsParallel()) {
		/*TODO deprecated*/
		a_clError = clEnqueueTask(a_clCommandQueue, a_oActiveProgram->a_clKernel, 0, NULL, &a_clDone);
		return;
	}

	a_clError = clEnqueueNDRangeKernel(a_clCommandQueue, a_oActiveProgram->a_clKernel, 
		a_oActiveProgram->a_iDimensions, a_oActiveProgram->a_aiOffset,
		a_oActiveProgram->a_aiGlobalRange, a_oActiveProgram->a_aiLocalRange, 0, NULL, &a_clDone);

	if (a_clError != CL_SUCCESS) { return;  }
	
	clWaitForEvents(1, &a_clDone);
}

void MOpenCLManager::Fx_LoadArguments()
{
	for(OpenCLArgument* oArgument : a_oActiveProgram->a_lArguments)
	{
		if(!oArgument) { continue; }
		if(!oArgument->a_bIsRaw) {
			a_clError = clSetKernelArg(a_oActiveProgram->a_clKernel, oArgument->a_iArgument, sizeof(cl_mem), &oArgument->a_clValue);
		}else {
			a_clError = clSetKernelArg(a_oActiveProgram->a_clKernel, oArgument->a_iArgument, oArgument->a_iSize, oArgument->a_ptrValue);
		}
		if (a_clError != CL_SUCCESS) {
			a_mLogManager->Warning(0,"[MOpenCLManager.Fx_LoadArguments] unable to set argument %d for program %d.",oArgument->a_iArgument, a_iActiveProgram);
			return;
		}
	}
}

void MOpenCLManager::Fx_DestroyArgument(OpenCLArgument* oArgument)
{
	if(!oArgument->a_bIsRaw) { clReleaseMemObject(oArgument->a_clValue); }
	delete oArgument;
}

void MOpenCLManager::Fx_DestroyProgram(OpenCLProgram* oProgram)
{
	if(!oProgram) { return; }

	a_clError = clReleaseKernel(oProgram->a_clKernel);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0,"[MOpenCLManager.Fx_DestroyProgram] unable to release kernel of program %d.",oProgram->a_iProgram);
		return;
	}
	
	a_clError = clReleaseProgram(oProgram->a_clProgram);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0,"[MOpenCLManager.Fx_DestroyProgram] unable to release OpenCL program %d.",oProgram->a_iProgram);
		return;
	}
	
	for(OpenCLArgument* oArgument : oProgram->a_lArguments) {
		if(!oArgument) { continue; }
		Fx_DestroyArgument(oArgument);
	}
	
	delete oProgram;
}

unsigned int MOpenCLManager::Fx_CompileProgram(std::string sSource, std::string sFunction, unsigned int iArgumentCount)
{
	const char* csSource = sSource.c_str();

	cl_program clProgram = clCreateProgramWithSource(a_clContext, 1, &csSource, NULL, &a_clError);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.Fx_CompileProgram] unable to create OpenCL program.");
		return 0;
	}
	
	cl_uint clResult = clBuildProgram(clProgram, 1, &a_clDevice, NULL, NULL, NULL);
	if (clResult) {
		a_mLogManager->Error(0,"[MOpenCLManager.Fx_CompileProgram] unable to build OpenCL program %u.",clResult);
		return 0;
	}

	cl_kernel clKernel = clCreateKernel(clProgram, sFunction.c_str(), &a_clError);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Error(0, "[MOpenCLManager.Fx_CompileProgram] unable to create OpenCL kernel.");
		return 0;
	}

	unsigned int iProgram = (unsigned int)a_lPrograms.size();
	a_lPrograms.push_back(new OpenCLProgram(iProgram,clProgram,clKernel,sFunction,iArgumentCount,"",clResult));
	return iProgram;
}

unsigned int MOpenCLManager::LoadProgramFromFile(std::string sPath, unsigned int iArgumentCount)
{
	std::string sFunction = "entry";
	return LoadProgramFromFile(sPath,sFunction,iArgumentCount);
}

unsigned int MOpenCLManager::LoadProgramFromFile(std::string sPath, std::string sFunction, unsigned int iArgumentCount)
{
	std::string sSource;
	
	std::ifstream ssFile(sPath);
	if(!ssFile) {
		a_mLogManager->Warning(0, "[MOpenCLManager.LoadProgramFromFile] file %s does not exist.",sPath.c_str());
		return 0;
	}

	while (!ssFile.eof()) {
		char sLine[MOPENCL_READLINE_SIZE];
		ssFile.getline(sLine,MOPENCL_READLINE_SIZE-1);
		sSource += sLine;
	}

	return LoadProgramFromString(sSource,sFunction,iArgumentCount);
}

unsigned int MOpenCLManager::LoadProgramFromString(std::string sSource, std::string sFunction, unsigned int iArgumentCount)
{
	unsigned int iProgram = Fx_CompileProgram(sSource,sFunction,iArgumentCount);
	return iProgram;
}

void MOpenCLManager::SetArgument(unsigned int iArgument, void* ptrValue, unsigned int iSize, bool bIsRaw, bool bIsNull, unsigned int eType)
{
	if(!a_oActiveProgram) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetArgument] there is no OpenCL program used.");
		return;
	} else if (iArgument < 0 || iArgument >= a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetArgument] given argument is out of scope.");
		return;
	}else if(!bIsNull && (!ptrValue || iSize < 0)) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetArgument] given argument to set for OpenCL program is NULL or its size < 0.");
	}

	OpenCLArgument* oArgument = NULL;
	cl_mem clValue = NULL;
	if(!bIsRaw) {
		clValue = clCreateBuffer(a_clContext, eType, iSize, ptrValue, &a_clError);
		if (a_clError != CL_SUCCESS) {
			a_mLogManager->Warning(0, "[MOpenCLManager.SetArgument] Unable to create OpenCL buffer for program %d argument %d.",a_iActiveProgram,iArgument);
			return;
		}
		oArgument = new OpenCLArgument(iArgument, clValue, iSize, bIsRaw);
	} else {
		oArgument = new OpenCLArgument(iArgument, ptrValue, iSize, bIsRaw);
	}

	if(a_oActiveProgram->a_lArguments[iArgument]==NULL) { ++a_oActiveProgram->a_iNumberOfArgumentsSet; }
	else { Fx_DestroyArgument(a_oActiveProgram->a_lArguments[iArgument]); }
	a_oActiveProgram->a_lArguments[iArgument] = oArgument;
}

void MOpenCLManager::SetChuncksTo(unsigned int iDimensions, size_t* aiOffset, size_t* aiGlobalRange, size_t* aiLocalRange)
{
	if (!aiGlobalRange) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetChuncksTo] global range 0 or NULL.");
	}

	if (iDimensions <= 0) {
		a_mLogManager->Warning(0, "[MOpenCLManager.SetChuncksTo] dimensions less or equal to zero.");
	}

	a_oActiveProgram->a_iDimensions = iDimensions;
	a_oActiveProgram->a_aiOffset = aiOffset;
	a_oActiveProgram->a_aiGlobalRange = aiGlobalRange;
	a_oActiveProgram->a_aiLocalRange = aiLocalRange;
}

void MOpenCLManager::Use(unsigned int iProgram)
{
	if(a_iActiveProgram == iProgram){
		a_mLogManager->Warning(0, "[MOpenCLManager.Use] programm already used");
		return; 
	} else if(iProgram < 0 || a_lPrograms.size() <= iProgram) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Use] program %d not in scope.",iProgram);
		return; 
	}

	a_iActiveProgram = iProgram;
	a_oActiveProgram = a_lPrograms[iProgram];

	// ASSUMPTION MAPPING IS ALWAYS IN SYNC
	if(a_oActiveProgram==NULL)
	{
		a_mLogManager->Warning(0, "[MOpenCLManager.Use] OpenCL program is destroyed.");
		a_iActiveProgram = 0;
	}
}

void MOpenCLManager::UnUse()
{
	if(!a_oActiveProgram){
		a_mLogManager->Warning(0, "[MOpenCLManager.UnUse] no program is used.");
		return; 
	}
	a_iActiveProgram = 0;
	a_oActiveProgram = NULL;
}

void MOpenCLManager::Destroy(unsigned int iProgram)
{
	if(iProgram < 0 || a_lPrograms.size() <= iProgram)
	{
		a_mLogManager->Warning(0, "[MOpenCLManager.Destory] program %d not in scope.",iProgram);
		return; 
	}
	if(a_iActiveProgram == iProgram) { UnUse(); }
	Fx_DestroyProgram(a_lPrograms[iProgram]);
	a_lPrograms[iProgram] = NULL;
}

void MOpenCLManager::Compute()
{	
	if(!a_oActiveProgram) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Compute] there is no OpenCL program used.");
		return; 
	} else if(a_oActiveProgram->a_iNumberOfArgumentsSet < a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Compute] not all arguments are set (some are NULL).");
	}
	
	Fx_LoadArguments();
	if(a_clError != CL_SUCCESS) { return; }

	Fx_EnqueueTask();
	if(a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Compute] unable to enqueue program kernel.");
		return;
	}
	a_bIsRunning = true;
}

void MOpenCLManager::Result(unsigned int iArgument, void* ptrValue, bool bIsWaitForCompleteResults)
{	
	if(!a_oActiveProgram) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Result] there is no OpenCL program used.");
		return; 
	} else if(!a_bIsRunning) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Result] OpenCL program is not running.");
		return;
	} else if(iArgument < 0 || iArgument >= a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Result] given argument is out of scope.");
	}

	OpenCLArgument* oArgument = a_oActiveProgram->a_lArguments[iArgument];

	if(oArgument->a_bIsRaw){
		a_mLogManager->Warning(0, "[MOpenCLManager.Result] given argument %d is is a raw value.",iArgument);
		return;
	}

	//float* temp2 = (float*)ptrValue;
	//unsigned int k2 = 4*sizeof(float);
	//a_mLogManager->LogF(LOG_DEBUG,0,"Debugging %f,%f,%f,%f",temp2[k2+0],temp2[k2+1],temp2[k2+2],temp2[k2+3]);

	//ptrValue = clEnqueueMapBuffer(a_clCommandQueue,oArgument->a_clValue,bIsWaitForCompleteResults,CL_MAP_READ,0,oArgument->a_iSize,0,NULL,NULL,&a_clError);
	a_clError = clEnqueueReadBuffer(a_clCommandQueue, oArgument->a_clValue , bIsWaitForCompleteResults, 0, oArgument->a_iSize, ptrValue, 0, NULL, NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Result] unable to retrieve result from command queue.");
		return;
	}
}

void MOpenCLManager::Finish()
{
	a_clError = clFinish(a_clCommandQueue);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Warning(0, "[MOpenCLManager.Finish] unable to successfully await finish of command queue completion.");
		return;
	}
}

unsigned int MOpenCLManager::GetActiveProgram()
{
	return a_iActiveProgram;
}

char* MOpenCLManager::Fx_MapErrorCode(unsigned int iErrorCode)
{
	switch (iErrorCode)
	{
		case 0: return "CL_SUCCESS";
		case -1: return "CL_DEVICE_NOT_FOUND";
		case -2: return "CL_DEVICE_NOT_AVAILABLE";
		case -3: return "CL_COMPILER_NOT_AVAILABLE";
		case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
		case -5: return "CL_OUT_OF_RESOURCES";
		case -6: return "CL_OUT_OF_HOST_MEMORY";
		case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
		case -8: return "CL_MEM_COPY_OVERLAP";
		case -9: return "CL_IMAGE_FORMAT_MISMATCH";
		case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
		case -11: return "CL_BUILD_PROGRAM_FAILURE";
		case -12: return "CL_MAP_FAILURE";
		case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
		case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
		case -15: return "CL_COMPILE_PROGRAM_FAILURE";
		case -16: return "CL_LINKER_NOT_AVAILABLE";
		case -17: return "CL_LINK_PROGRAM_FAILURE";
		case -18: return "CL_DEVICE_PARTITION_FAILED";
		case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
		case -30: return "CL_INVALID_VALUE";
		case -31: return "CL_INVALID_DEVICE_TYPE";
		case -32: return "CL_INVALID_PLATFORM";
		case -33: return "CL_INVALID_DEVICE";
		case -34: return "CL_INVALID_CONTEXT";
		case -35: return "CL_INVALID_QUEUE_PROPERTIES";
		case -36: return "CL_INVALID_COMMAND_QUEUE";
		case -37: return "CL_INVALID_HOST_PTR";
		case -38: return "CL_INVALID_MEM_OBJECT";
		case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
		case -40: return "CL_INVALID_IMAGE_SIZE";
		case -41: return "CL_INVALID_SAMPLER";
		case -42: return "CL_INVALID_BINARY";
		case -43: return "CL_INVALID_BUILD_OPTIONS";
		case -44: return "CL_INVALID_PROGRAM";
		case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
		case -46: return "CL_INVALID_KERNEL_NAME";
		case -47: return "CL_INVALID_KERNEL_DEFINITION";
		case -48: return "CL_INVALID_KERNEL";
		case -49: return "CL_INVALID_ARG_INDEX";
		case -50: return "CL_INVALID_ARG_VALUE";
		case -51: return "CL_INVALID_ARG_SIZE";
		case -52: return "CL_INVALID_KERNEL_ARGS";
		case -53: return "CL_INVALID_WORK_DIMENSION";
		case -54: return "CL_INVALID_WORK_GROUP_SIZE";
		case -55: return "CL_INVALID_WORK_ITEM_SIZE";
		case -56: return "CL_INVALID_GLOBAL_OFFSET";
		case -57: return "CL_INVALID_EVENT_WAIT_LIST";
		case -58: return "CL_INVALID_EVENT";
		case -59: return "CL_INVALID_OPERATION";
		case -60: return "CL_INVALID_GL_OBJECT";
		case -61: return "CL_INVALID_BUFFER_SIZE";
		case -62: return "CL_INVALID_MIP_LEVEL";
		case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
		case -64: return "CL_INVALID_PROPERTY";
		case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
		case -66: return "CL_INVALID_COMPILER_OPTIONS";
		case -67: return "CL_INVALID_LINKER_OPTIONS";
		case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";
		default: return "UNKNOWN";
	}
	return "NOTPOSSIBLE";
}