#include "MComputationManager.hpp"
#include "CMonoscopic.hpp"

MComputationManager* MComputationManager::a_oInstance = 0;

MComputationManager::MComputationManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;
}

MComputationManager::~MComputationManager()
{

}

MComputationManager* MComputationManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MComputationManager();
	return a_oInstance;
}

void MComputationManager::SetUp()
{
	if(a_bIsSetUp) {
		return; 
	}
	a_bIsShutDown = false;

	// TODO MComputationManager SetUp

	a_bIsSetUp = true;
}

void MComputationManager::ShutDown()
{
	if(a_bIsShutDown) {
		return; 
	}
	a_bIsSetUp = false;

	// TODO MComputationManager ShutDown

	a_bIsShutDown = true;
}

void MComputationManager::Run()
{
	if(!a_bIsSetUp) {
		return;
	}

	// TODO DO DIFFERENT
	IComputation* oComputation = new CMonoscopic();
	oComputation->Run();
}
