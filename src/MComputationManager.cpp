#include "MComputationManager.hpp"
#include "CMonoscopic.hpp"
#include "CStereoscopic.hpp"

MComputationManager* MComputationManager::a_oInstance = 0;

MComputationManager::MComputationManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_mLogManager = MLogManager::GetInstance();
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
		a_mLogManager->Warning(0,"[MComputationManager.SetUp] already setup.");
		return;
	}
	a_bIsShutDown = false;

	// TODO MComputationManager SetUp

	a_bIsSetUp = true;
	a_mLogManager->Success(0,"[MComputationManager.SetUp] done.");
}

void MComputationManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0,"[MComputationManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;

	// TODO MComputationManager ShutDown

	a_bIsShutDown = true;
	a_mLogManager->Success(0,"[MComputationManager.ShutDown] done.");
}

void MComputationManager::Run()
{
	if(!a_bIsSetUp) {
		return;
	}

	// TODO DO DIFFERENT
	IComputation* oComputation = new CStereoscopic();
	oComputation->Run();
}
