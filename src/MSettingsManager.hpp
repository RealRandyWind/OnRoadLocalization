#ifndef MSETTINGSMANAGER_HPP
#define MSETTINGSMANAGER_HPP
#include "MLogManager.hpp"
#include <string>
#include <vector>

class MSettingsManager;
class MSettingsManager
{
private:
	MSettingsManager();
	static MSettingsManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	MLogManager* a_mLogManager;

	//std::vector<Settings*> a_lSettings;

	unsigned int Fx_CompileSettings(std::string sSettings);
public:
	~MSettingsManager();
	static MSettingsManager* GetInstance();

	void SetUp();
	void ShutDown();

	unsigned int LoadSettingsFromFile(std::string sPath);
	unsigned int LoadSettingsFromString(std::string sSettings);

	void* Get(unsigned int iSettings, std::string sSetting);
};
#endif