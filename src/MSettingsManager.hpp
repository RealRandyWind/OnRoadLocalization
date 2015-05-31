#ifndef MSETTINGSMANAGER_HPP
#define MSETTINGSMANAGER_HPP
#include <string>

class MSettingsManager;
class MSettingsManager
{
private:
	MSettingsManager();
	static MSettingsManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown; 
public:
	~MSettingsManager();
	static MSettingsManager* GetInstance();

	void SetUp();
	void ShutDown();

	unsigned int LoadSettingsFromFile(std::string sPath);
	unsigned int LoadSettingsFromString(std::string sString);

	void* Get(unsigned int iSettings, std::string sSetting);
};
#endif