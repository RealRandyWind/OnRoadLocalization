#include "MRenderManager.hpp"

MRenderManager* MRenderManager::a_oInstance = 0;

MRenderManager::MRenderManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_mLogManager = MLogManager::GetInstance();
}

MRenderManager::~MRenderManager()
{

}

MRenderManager* MRenderManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MRenderManager();
	return a_oInstance;
}

void MRenderManager::SetUp()
{
	if(a_bIsSetUp) {
		a_mLogManager->Warning(0, "[MRenderManager.SetUp] already setup.");
		return; 
	}
	a_bIsShutDown = false;

	// TODO MRenderManager SetUp

	a_bIsSetUp = true;
	a_mLogManager->Success(0, "[MRenderManager.SetUp] done.");
}

void MRenderManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0, "[MRenderManager.ShutDown] already shutdown.");
		return;
	}
	a_bIsSetUp = false;

	// TODO MRenderManager ShutDown

	a_bIsShutDown = true;
	a_mLogManager->Success(0, "[MRenderManager.ShutDown] done.");
}