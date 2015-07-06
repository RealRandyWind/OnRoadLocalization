#include "Main.hpp"

Main* Main::a_oInstance = 0;
int Main::a_iExitCode = 0;

Main::Main()
{
	a_bIsRunning = false;

	a_mLogManager = MLogManager::GetInstance();
	a_mSettingsManager = MSettingsManager::GetInstance();
	a_mMemoryManager = MMemoryManager::GetInstance();
	a_mOpenCLManager = MOpenCLManager::GetInstance();
	a_mSDLManager = MSDLManager::GetInstance();
	a_mRenderManager = MRenderManager::GetInstance();
	a_mResourceManager = MResourceManager::GetInstance();
	a_mComputationManager = MComputationManager::GetInstance();
	
}
Main::~Main()
{

}

Main* Main::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new Main();
	return a_oInstance;
}

void Main::ErrorShutDown(int iExitCode)
{
	if(a_oInstance) { a_oInstance->Fx_ShutDown(); }
	a_iExitCode = (iExitCode ? iExitCode : 1);
	exit(a_iExitCode);
}

int Main::Entry()
{
	if(a_bIsRunning) {
		printf(LOG_COLOR_WARNING "\n_WARNING: [Main.Entry] entry already called before." LOG_COLOR_RESET);
		return 1;
	}
	a_bIsRunning = true;

	a_mLogManager->SetUp();
	a_mSettingsManager->SetUp();
	a_mMemoryManager->SetUp();
	a_mOpenCLManager->SetUp();
	a_mSDLManager->SetUp();
	a_mRenderManager->SetUp();
	a_mResourceManager->SetUp();
	a_mComputationManager->SetUp();
	
	a_mComputationManager->Run();

	Fx_ShutDown();

	return a_iExitCode;
}

void Main::Fx_ShutDown()
{
	a_mComputationManager->ShutDown();
	a_mResourceManager->ShutDown();
	a_mRenderManager->ShutDown();
	a_mSDLManager->ShutDown();
	a_mOpenCLManager->ShutDown();
	a_mMemoryManager->ShutDown();
	a_mSettingsManager->ShutDown();
	a_mLogManager->ShutDown();

	a_bIsRunning = false;
}
