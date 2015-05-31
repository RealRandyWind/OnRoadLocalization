#include "MSettingsManager.hpp"

MSettingsManager* MSettingsManager::a_oInstance = 0;

MSettingsManager::MSettingsManager()
{
	a_bIsShutDown = false;
	a_bIsSetUp = false;
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
		return; 
	}
	a_bIsShutDown = false;

	// TODO MSettingsManager SetUp

	a_bIsSetUp = true;
}

void MSettingsManager::ShutDown()
{
	if(a_bIsShutDown) {
		return; 
	}
	a_bIsSetUp = false;

	// TODO MSettingsManager ShutDown

	a_bIsShutDown = true;
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