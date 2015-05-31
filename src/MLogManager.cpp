#include "MLogManager.hpp"
#include "Main.hpp"

#ifdef __linux__
#include <unistd.h>
#endif

MLogManager* MLogManager::a_oInstance = 0;

MLogManager::MLogManager()
{
	a_bIsShutDown = true;
	a_bIsSetUp = false;
	a_iLogCounter = 0;
	SetLogFile("default.log"); 
	IsPrint(true,LOG_ALL);
}

MLogManager::~MLogManager()
{

}

MLogManager* MLogManager::GetInstance()
{
	if(a_oInstance) { return a_oInstance; }
	a_oInstance = new MLogManager();
	return a_oInstance;
}

void MLogManager::SetUp()
{
	if(a_bIsSetUp) {
		Log("Call to 'SetUp' of MLogManager, but setup already",0,LOG_WARNING);
		return; 
	}
	a_bIsShutDown = false;

	// TODO MLogManager setup

	a_bIsSetUp = true;
	Log("SetUp MLogManager is done",0,LOG_SUCCESS);
}

void MLogManager::ShutDown()
{
	if(a_bIsShutDown) {
		printf(LOG_COLOR_WARNING "-Log.Warning:Call to 'ShutDown' of MLogManager, but shutdown already" LOG_COLOR_RESET);
		return; 
	}
	a_bIsSetUp = false;

	for (int i=0; i<LOG_TYPE_COUNT; ++i) { 
		if(a_lFileStreames[i].is_open()) {
			a_lFileStreames[i].flush();
			a_lFileStreames[i].close();
		}
	}
	//a_lFileStreames[iFile]

	// TODO MLogManager shutdown

	a_bIsShutDown = true;
	printf(LOG_COLOR_SUCCESS "-Log.Success:ShutDown MLogManager is done\n" LOG_COLOR_RESET);
}


void MLogManager::SetLogFile(std::string sFilePath)
{
	SetLogFile(sFilePath,LOG_ANY);
}

void MLogManager::SetLogFile(std::string sFilePath, unsigned int eType)
{

	if(LOG_ERROR & eType) { Fx_SetLogFile(sFilePath,0); }
	if(LOG_SUCCESS & eType) { Fx_SetLogFile(sFilePath,1); }
	if(LOG_INFO & eType) { Fx_SetLogFile(sFilePath,2); }
	if(LOG_WARNING & eType) { Fx_SetLogFile(sFilePath,3); }
	if(LOG_DEBUG & eType) { Fx_SetLogFile(sFilePath,4); }
}

void MLogManager::Fx_SetLogFile(std::string sFilePath, unsigned int iFile)
{
	a_lTypeFilePaths[iFile] = sFilePath;
	if(a_lFileStreames[iFile].is_open()) { a_lFileStreames[iFile].close(); }
	a_lFileStreames[iFile].open(sFilePath,std::fstream::out | std::fstream::app);
}

void MLogManager::Fx_WriteLog(std::string sString, unsigned int iFile)
{
	a_lFileStreames[iFile] << a_iLogCounter-1 << ":\t" << sString << "\n";
	if(!(a_iLogCounter % LOG_FILE_FLUSHRATE)) { a_lFileStreames[iFile].flush(); }
}

void MLogManager::Fx_PrintLog(std::string sString)
{
	sString += LOG_COLOR_RESET;
	printf("%s\n",sString.c_str());
}

void MLogManager::Fx_EvaluateLog(std::string sLog, unsigned int iCode, unsigned int eType)
{
	if(a_bIsShutDown) {
		printf(LOG_COLOR_WARNING "-Log.Warning:Call to Log of MLogManager while allready shutdown\n" LOG_COLOR_RESET);
		return; 
	}

	if(!(LOG_ANY & eType)) { Log("Invalid enum type on evaluation",0,LOG_WARNING); return; }
	std::string sString;
	++a_iLogCounter;
	
	std::string sPrefix = LOG_LOG_STR;
	if(iCode) { sPrefix +="Code[" + std::to_string(iCode) + "]."; }
	
	if(LOG_SUCCESS & eType) {
		sString = sPrefix + LOG_SUCCESS_STR + sLog;
		Fx_WriteLog(sString,1);
		if(a_sTypeIsPrint[1]) { Fx_PrintLog(LOG_COLOR_SUCCESS + sString); }
	}
	
	if(LOG_INFO & eType) {
		sString = sPrefix + LOG_INFO_STR + sLog;
		Fx_WriteLog(sString,2);
		if(a_sTypeIsPrint[2]) { Fx_PrintLog(LOG_COLOR_INFO + sString); }
	}
	
	if(LOG_WARNING & eType) {
		sString = sPrefix + LOG_WARNING_STR + sLog;
		Fx_WriteLog(sString,3);
		if(a_sTypeIsPrint[3]) { Fx_PrintLog(LOG_COLOR_WARNING + sString); }

#ifdef __linux__
		if(LOG_WARNING_USLEEP_TIME){
			if(usleep(LOG_WARNING_USLEEP_TIME)<0) { 
				Log("Usleep not working or canceled, consider to define LOG_WARNING_USLEEP_TIME to zero.",0,LOG_ERROR);
			}
		}
#endif
	}
	
	if(LOG_DEBUG & eType) {
		sString = "  " + sPrefix + LOG_DEBUG_STR + sLog;
		Fx_WriteLog(sString,4);
		if(a_sTypeIsPrint[4]) { Fx_PrintLog(LOG_COLOR_DEBUG + sString); }
	}

	if(LOG_ERROR & eType) {
		sString = sPrefix + LOG_ERROR_STR + sLog;
		Fx_WriteLog(sString,0);
		if(a_sTypeIsPrint[0]) { Fx_PrintLog(LOG_COLOR_ERROR + sString); }
		Main::ErrorShutDown();
	}
}

bool MLogManager::IsPrint(unsigned int eType)
{	
	bool bAllIsPrint = true;
	
	if(LOG_ERROR & eType) { bAllIsPrint &= a_sTypeIsPrint[0]; }
	if(LOG_SUCCESS & eType) { bAllIsPrint &= a_sTypeIsPrint[1]; }
	if(LOG_INFO & eType) { bAllIsPrint &= a_sTypeIsPrint[2]; }
	if(LOG_WARNING & eType) { bAllIsPrint &= a_sTypeIsPrint[3]; }
	if(LOG_DEBUG & eType) { bAllIsPrint &= a_sTypeIsPrint[4]; }
	
	return bAllIsPrint;
}

bool MLogManager::IsPrint(bool bIsPrint, unsigned int eType)
{
	if(LOG_ERROR & eType) { a_sTypeIsPrint[0] = bIsPrint;}
	if(LOG_SUCCESS & eType) { a_sTypeIsPrint[1] = bIsPrint; }
	if(LOG_INFO & eType) { a_sTypeIsPrint[2] = bIsPrint; }
	if(LOG_WARNING & eType) { a_sTypeIsPrint[3] = bIsPrint; }
	if(LOG_DEBUG & eType) { a_sTypeIsPrint[4] = bIsPrint; }

	return bIsPrint;
}
	

/* Log Functions */
void MLogManager::LogF(unsigned int eType, unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf(sBuffer,sFormat, vaArgs);
	va_end(vaArgs);
	Log(std::string(sBuffer),iCode,eType);
}

void MLogManager::Log(std::string sMessege)
{
	Log(sMessege,0,LOG_DEFAULT);
}

void MLogManager::Log(std::string sMessege, unsigned int iCode)
{
	Log(sMessege,iCode,LOG_DEFAULT);
}

void MLogManager::Log(std::string sMessege, unsigned int iCode, unsigned int eType)
{
	Fx_EvaluateLog(sMessege,iCode,eType);
}


void MLogManager::Log(std::string sMessege, std::string sString, unsigned int iCode, unsigned int eType)
{
	std::string sLog = sMessege + ": s(\"" + sString + "\")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, char* sString, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::string(sString);
	std::string sLog = sMessege + ": s(\"" + sValue + "\")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, char cChar, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::string(1,cChar);
	std::string sLog = sMessege + ": c('" + sValue + "')";
	Fx_EvaluateLog(sLog,iCode,eType) ;
}

void MLogManager::Log(std::string sMessege, unsigned char cChar, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::string(1,cChar);
	std::string sLog = sMessege + ": c('" + sValue + "')";
	Fx_EvaluateLog(sLog,iCode,eType);
}


void MLogManager::Log(std::string sMessege, unsigned int iNumber, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::to_string(iNumber);
	std::string sLog = sMessege + ": i(" + sValue + ")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, int iNumber, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::to_string(iNumber);
	std::string sLog = sMessege + ": i(" + sValue + ")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, long iNumber, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::to_string(iNumber);
	std::string sLog = sMessege + ": i(" + sValue + ")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, double dNumber, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::to_string(dNumber);
	std::string sLog = sMessege + ": d(" + sValue + ")";
	Fx_EvaluateLog(sLog,iCode,eType);
}

void MLogManager::Log(std::string sMessege, float dNumber, unsigned int iCode, unsigned int eType)
{
	std::string sValue = std::to_string(dNumber);
	std::string sLog = sMessege + ": d(" + sValue + ")";
	Fx_EvaluateLog(sLog,iCode,eType);
}
