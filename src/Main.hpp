#ifndef MAIN_HPP
#define MAIN_HPP
#include "MSDLManager.hpp"
#include "MOpenCLManager.hpp"
#include "MComputationManager.hpp"
#include "MResourceManager.hpp"
#include "MRenderManager.hpp"
#include "MMemoryManager.hpp"
#include "MSettingsManager.hpp"
#include "MLogManager.hpp"

class Main;
class Main
{
private:
	Main();
	static Main* a_oInstance;
	static int a_iExitCode;
	bool a_bIsRunning;

	MLogManager* a_mLogManager;
	MSettingsManager* a_mSettingsManager;
	MMemoryManager* a_mMemoryManager;
	MOpenCLManager* a_mOpenCLManager;
	MSDLManager* a_mSDLManager;
	MRenderManager* a_mRenderManager;
	MResourceManager* a_mResourceManager;
	MComputationManager* a_mComputationManager;
	
	void Fx_ShutDown();
public:
	~Main();
	static Main* GetInstance();
	static void ErrorShutDown(int iExitCode = 1);

	int Entry();
};
#endif
