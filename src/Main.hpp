#ifndef MAIN_HPP
#define MAIN_HPP
#include "MOpenCLManager.hpp"
#include "MComputationManager.hpp"
#include "MResourceManager.hpp"
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
	MOpenCLManager* a_mOpenCLManager;
	MComputationManager* a_mComputationManager;
	MResourceManager* a_mResourceManager;
	
	void Fx_ShutDown();
public:
	~Main();
	static Main* GetInstance();
	static void ErrorShutDown(int iExitCode = 1);

	int Entry();
};
#endif
