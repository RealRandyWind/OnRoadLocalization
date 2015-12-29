#include "MSettingsManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

#define MSETTINGS_READLINE_SIZE 256
#define MSETTINGS_SETTINGNAME_SIZE 256

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
	std::string sSettings;

	std::ifstream ssFile(sPath);
	if (!ssFile) {
		a_mLogManager->Warning(0, "[MSettingsManager.LoadSettingsFromFile] file %s does not exist.", sPath.c_str());
		return 0;
	}

	while (!ssFile.eof()) {
		char sLine[MSETTINGS_READLINE_SIZE];
		ssFile.getline(sLine, MSETTINGS_READLINE_SIZE - 1);
		sSettings += sLine;
	}

	return LoadSettingsFromString(sSettings);
}

unsigned int MSettingsManager::LoadSettingsFromString(std::string sSettings)
{
	unsigned int iSettings = Fx_CompileSettings(sSettings);
	return iSettings;
}

void* MSettingsManager::Get(unsigned int iSettings, std::string sSetting)
{
	// TODO MSettingsManager Get

	return NULL;
}

unsigned int MSettingsManager::Fx_CompileSettings(std::string sSettings)
{
	/*Settings* oSettings = new Settings();

	unsigned int iSettings = (unsigned int)a_lSettings.size();
	a_lSettings.push_back(oSettings);
	return iSettings;*/
	return -1;
}