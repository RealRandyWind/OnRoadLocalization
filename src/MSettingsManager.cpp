#include "MSettingsManager.hpp"

MSettingsManager* MSettingsManager::a_oInstance = 0;

MSettingsManager::MSettingsManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;

	a_mLogManager = MLogManager::GetInstance();
}

MSettingsManager::~MSettingsManager()
{

}

MSettingsManager* MSettingsManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MSettingsManager();
	return a_oInstance;
}

void MSettingsManager::SetUp()
{
	if(a_bIsSetUp) {
		a_mLogManager->Warning(0, "[MSettingsManager.SetUp] already setup.");
		return; 
	}
	a_bIsShutDown = false;

	// TODO MSettingsManager SetUp

	a_bIsSetUp = true;
	a_mLogManager->Success(0, "[MSettingsManager.SetUp] done.");
}

void MSettingsManager::ShutDown()
{
	if(a_bIsShutDown) {
		a_mLogManager->Warning(0, "[MSettingsManager.ShutDown] already shutdown.");
		return; 
	}
	a_bIsSetUp = false;

	// TODO MSettingsManager ShutDown

	a_bIsShutDown = true;
	a_mLogManager->Success(0, "[MSettingsManager.ShutDown] done.");
}

unsigned int MSettingsManager::LoadSettingsFromFile(std::string sPath)
{
	// TODO MSettingsManager LoadSettingsFromFile

	return -1;
}

unsigned int MSettingsManager::LoadSettingsFromString(std::string sString)
{
	// TODO MSettingsManager LoadSettingsFromString

	return -1;
}

void* MSettingsManager::Get(unsigned int iSettings, std::string sSetting)
{
	// TODO MSettingsManager Get

	return NULL;
}