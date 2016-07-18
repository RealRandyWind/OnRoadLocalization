#include "MMemoryManager.hpp"

MMemoryManager* MMemoryManager::a_oInstance = 0;

MMemoryManager::MMemoryManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_iBlockCounter = 0;

	a_mLogManager = MLogManager::GetInstance();
}

MMemoryManager::~MMemoryManager()
{

}

MMemoryManager* MMemoryManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MMemoryManager();
	return a_oInstance;
}

void MMemoryManager::SetUp()
{
	if(a_bIsSetUp) {
		a_mLogManager->Warning(0,"[MMemoryManager.SetUp] already setup.");
		return;
	}
	a_bIsShutDown = false;

	// TODO MMemoryManager SetUp

	a_bIsSetUp = true;
	a_mLogManager->Success(0,"[MMemoryManager.SetUp] done.");
}

void MMemoryManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0,"[MMemoryManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;

	if (a_iBlockCounter) {
		a_mLogManager->Warning(0, "[MMemoryManager.ShutDown] some blocks are not dealocated at all or not by this manager.");
		return;
	}

	a_bIsShutDown = true;
	a_mLogManager->Success(0,"[MMemoryManager.ShutDown] done.");
}

void* MMemoryManager::Allocate(size_t nSize)
{
	void* oReturn = NULL;
	
	oReturn = malloc(nSize);
	if (!oReturn) {
		a_mLogManager->Error(0,"[MMemoryManager.Allocate] unable to allocate memory.");
		return NULL;
	}
	
	++a_iBlockCounter;

	return oReturn;
}

void* MMemoryManager::ReAllocate(void* oTarget, size_t nSize)
{
	if (!oTarget) {
		a_mLogManager->Warning(0,"[MMemoryManager.ReAllocate] target is NULL.");
		return NULL;
	}

	void* oReturn = NULL;
	
	oReturn = realloc(oTarget, nSize);
	if (!oReturn) {
		a_mLogManager->Error(0,"[MMemoryManager.ReAllocate] unable to reallocate memory.");
		return NULL;
	}

	return oReturn;
}

void MMemoryManager::DeAllocate(void* oTarget)
{
	if (!oTarget) {
		a_mLogManager->Warning(0,"[MMemoryManager.DeAllocate] target is NULL.");
		return;
	}

	delete[] oTarget;
	
	--a_iBlockCounter;
}