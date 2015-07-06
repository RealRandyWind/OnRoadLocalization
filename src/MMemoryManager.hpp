#ifndef MMEMORYMANAGER_HPP
#define MMEMORYMANAGER_HPP
#include "MLogManager.hpp"

class MMemoryManager;
class MMemoryManager
{
private:
	MMemoryManager();
	static MMemoryManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	MLogManager* a_mLogManager;

	unsigned int a_iBlockCounter;
	/*void Fx_FreeAll();*/
public:
	~MMemoryManager();
	static MMemoryManager* GetInstance();

	void SetUp();
	void ShutDown();

	void* Allocate(size_t iSize);
	void* ReAllocate(void* oTarget, size_t iSize);
	void DeAllocate(void* oTarget);
	/*void Defragmentate();*/
};
#endif