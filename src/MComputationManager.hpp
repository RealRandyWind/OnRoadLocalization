#ifndef MCOMPUTATIONMANAGER_HPP
#define MCOMPUTATIONMANAGER_HPP
#include "IComputation.hpp"

class MComputationManager;
class MComputationManager
{
private:
	MComputationManager();
	static MComputationManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown; 
public:
	~MComputationManager();
	static MComputationManager* GetInstance();

	void SetUp();
	void ShutDown();
	void Run();
};
#endif
