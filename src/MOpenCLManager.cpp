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
		a_mLogManager->Log("Call to 'SetUp' of MOpenCLManager, but setup already",0,LOG_WARNING);
		return; 
	}
	a_bIsShutDown = false;

	
	a_clError = clGetPlatformIDs(1,&a_clPlatform,NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to find any platform",0,LOG_ERROR);
		return;
	}

	a_clError = clGetDeviceIDs(a_clPlatform,CL_DEVICE_TYPE_GPU,1,&a_clDevice,NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to find any device",0,LOG_ERROR);
		return;
	}

	a_clContext = clCreateContext(NULL, 1, &a_clDevice, NULL, NULL, &a_clError);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to create context for device",0,LOG_WARNING);
		return;
	}

	a_clCommandQueue = clCreateCommandQueue(a_clContext, a_clDevice, 0, NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to create command queue for device",0,LOG_WARNING);
		return;
	}

	
	a_clError = clGetDeviceInfo(a_clDevice, CL_DEVICE_MAX_MEM_ALLOC_SIZE , sizeof(cl_ulong), &a_clMaxMemSize, 0);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to retrieve maximum memory size available by current device",0,LOG_WARNING);
		return;
	}
	a_mLogManager->LogF(LOG_INFO,0,"MOpenCLManager device has %lu of memmory available",a_clMaxMemSize);


	a_bIsSetUp = true;
	a_mLogManager->Log("SetUp MOpenCLManager is done",0,LOG_SUCCESS);
}

void MOpenCLManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Log("Call to 'ShutDown' of MOpenCLManager, but shutdown already",0,LOG_WARNING);
		return; 
	}
	a_bIsSetUp = false;

	for (OpenCLProgram* oProgram : a_lPrograms) { Fx_DestroyProgram(oProgram); }

	a_clError = clReleaseContext(a_clContext);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to release context for device",0,LOG_WARNING);
		return;
	}
	a_clContext = 0;

	a_clError = clReleaseCommandQueue(a_clCommandQueue);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to release command queue for device",0,LOG_WARNING);
		return;
	}
	a_clCommandQueue = 0;

	a_clDevice = 0;
	a_clPlatform = 0;
	a_clMaxMemSize = 0;

	a_bIsShutDown = true;
	a_mLogManager->Log("ShutDown MOpenCLManager is done",0,LOG_SUCCESS);
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
			a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager unable to set argument %d for program %d",oArgument->a_iArgument, a_iActiveProgram);
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
		a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager unable to release kernel of program %d",oProgram->a_iProgram);
		return;
	}
	
	a_clError = clReleaseProgram(oProgram->a_clProgram);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager unable to release OpenCL program %d",oProgram->a_iProgram);
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
		a_mLogManager->Log("MOpenCLManager unable to create OpenCL program",0,LOG_WARNING);
		return 0;
	}
	
	cl_uint clResult = clBuildProgram(clProgram, 1, &a_clDevice, NULL, NULL, NULL);
	if (clResult) {
		a_mLogManager->Log("MOpenCLManager unable to build OpenCL program",clResult,0,LOG_WARNING);
		return 0;
	}

	cl_kernel clKernel = clCreateKernel(clProgram, sFunction.c_str(), &a_clError);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to create OpenCL kernel",0,LOG_WARNING);
		return 0;
	}

	unsigned int iProgram = a_lPrograms.size();
	a_lPrograms.push_back(new OpenCLProgram(iProgram,clProgram,clKernel,sFunction,iArgumentCount,"",clResult));
	return iProgram;
}

unsigned int MOpenCLManager::LoadProgramFromFile(std::string sPath, unsigned int iArgumentCount)
{
	std::string sFunction = "main";
	return LoadProgramFromFile(sPath,sFunction,iArgumentCount);
}

unsigned int MOpenCLManager::LoadProgramFromFile(std::string sPath, std::string sFunction, unsigned int iArgumentCount)
{
	std::string sSource;
	
	std::ifstream ssFile(sPath);
	if(!ssFile) {
		a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager file %s does not exist",sPath.c_str());
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

void MOpenCLManager::SetArgument(unsigned int iArgument, void* ptrValue, size_t iSize,bool bIsRaw, bool bIsNull, unsigned int eType)
{
	if(!a_iActiveProgram) {
		a_mLogManager->Log("Call to 'SetArgument' MOpenCLManager, but there is no OpenCL program used",0,LOG_WARNING);
		return;
	} else if (iArgument < 0 || iArgument >= a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Log("Call to 'SetArgument' MOpenCLManager, but given argument is out of scope",0,LOG_WARNING);
		return;
	}else if(!bIsNull && (!ptrValue || iSize < 0)) {
		a_mLogManager->Log("Call to 'SetArgument' MOpenCLManager, but given argument to set for OpenCL program is NULL or its size < 0",0,LOG_WARNING);
	}

	OpenCLArgument* oArgument = NULL;
	cl_mem clValue = NULL;
	if(!bIsRaw) {
		clValue = clCreateBuffer(a_clContext, eType, iSize, ptrValue, &a_clError);
		if (a_clError != CL_SUCCESS) {
			a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager Unable to create OpenCL buffer for program %d argument %d",a_iActiveProgram,iArgument);
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
	a_oActiveProgram->a_iDimensions = iDimensions;
	a_oActiveProgram->a_aiOffset = aiOffset;
	a_oActiveProgram->a_aiGlobalRange = aiGlobalRange;
	a_oActiveProgram->a_aiLocalRange = aiLocalRange;
}

void MOpenCLManager::Use(unsigned int iProgram)
{
	if(!iProgram) { 
		a_mLogManager->Log("Call to 'Use' MOpenCLManager with 0 id, use UnUse instead",0,LOG_WARNING);
		UnUse(); return; 
	}

	if(a_iActiveProgram == iProgram){
		a_mLogManager->Log("Call to 'Use' of MOpenCLManager, but programm already used",0,LOG_WARNING);
		return; 
	} else if(iProgram < 0 || a_lPrograms.size() <= iProgram) {
		a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager program %d not in scope",iProgram);
		return; 
	}

	a_iActiveProgram = iProgram;
	a_oActiveProgram = a_lPrograms[iProgram];

	// ASSUMPTION MAPPING IS ALWAYS IN SYNC
	if(a_oActiveProgram==NULL)
	{
		a_mLogManager->Log("Call to 'Use' of MOpenCLManager, but OpenCL program is destroyed",0,LOG_WARNING);
		a_iActiveProgram = 0;
	}
}

void MOpenCLManager::UnUse()
{
	if(!a_iActiveProgram){
		a_mLogManager->Log("Call to 'UnUse' of MOpenCLManager, but no program is used",0,LOG_WARNING);
		return; 
	}
	a_iActiveProgram = 0;
	a_oActiveProgram = NULL;
}

void MOpenCLManager::Destroy(unsigned int iProgram)
{
	if(iProgram < 0 || a_lPrograms.size() <= iProgram)
	{
		a_mLogManager->LogF(LOG_WARNING,0,"MOpenCLManager program %d not in scope",iProgram);
		return; 
	}
	if(a_iActiveProgram == iProgram) { UnUse(); }
	Fx_DestroyProgram(a_lPrograms[iProgram]);
	a_lPrograms[iProgram] = NULL;
}

void MOpenCLManager::Compute()
{	
	if(!a_iActiveProgram) {
		a_mLogManager->Log("Call to 'Compute' MOpenCLManager, but there is no OpenCL program used",0,LOG_WARNING);
		return; 
	} else if(a_oActiveProgram->a_iNumberOfArgumentsSet < a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Log("Call to 'Compute' MOpenCLManager, but not all arguments are set (some are NULL)",0,LOG_WARNING);
	}
	
	Fx_LoadArguments();
	if(a_clError != CL_SUCCESS) { return; }

	Fx_EnqueueTask();
	if(a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to enqueue program kernel",0,LOG_WARNING);
		return;
	}

	a_bIsRunning = true;
}

void MOpenCLManager::Result(unsigned int iArgument, void* ptrValue, bool bIsWaitForCompleteResults)
{	
	if(!a_iActiveProgram) {
		a_mLogManager->Log("Call to 'Result' MOpenCLManager, but there is no OpenCL program used",0,LOG_WARNING);
		return; 
	} else if(!a_bIsRunning) {
		a_mLogManager->Log("Call to 'Result' MOpenCLManager, but OpenCL program is not running",0,LOG_WARNING);
		return;
	} else if(iArgument < 0 || iArgument >= a_oActiveProgram->a_iArgumentCount) {
		a_mLogManager->Log("Call to 'Result' MOpenCLManager, but given argument is out of scope",0,LOG_WARNING);
	}

	OpenCLArgument* oArgument = a_oActiveProgram->a_lArguments[iArgument];

	if(oArgument->a_bIsRaw){
		a_mLogManager->LogF(LOG_WARNING,0,"Call to 'Result' MOpenCLManager, but given argument %d is is a raw value",iArgument);
		return;
	}

	//float* temp2 = (float*)ptrValue;
	//unsigned int k2 = 4*sizeof(float);
	//a_mLogManager->LogF(LOG_DEBUG,0,"Debugging %f,%f,%f,%f",temp2[k2+0],temp2[k2+1],temp2[k2+2],temp2[k2+3]);

	//ptrValue = clEnqueueMapBuffer(a_clCommandQueue,oArgument->a_clValue,bIsWaitForCompleteResults,CL_MAP_READ,0,oArgument->a_iSize,0,NULL,NULL,&a_clError);
	a_clError = clEnqueueReadBuffer(a_clCommandQueue, oArgument->a_clValue , bIsWaitForCompleteResults, 0, oArgument->a_iSize, ptrValue, 0, NULL, NULL);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to retrieve result from command queue",0,LOG_WARNING);
		return;
	}
}

void MOpenCLManager::Finish()
{
	a_clError = clFinish(a_clCommandQueue);
	if (a_clError != CL_SUCCESS) {
		a_mLogManager->Log("MOpenCLManager unable to successfully await finish of command queue completion.",0,LOG_WARNING);
		return;
	}
}

unsigned int MOpenCLManager::GetActiveProgram()
{
	return a_iActiveProgram;
}
