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
		Warning(0,"[MLogmanager.SetUp] already setup.");
		return; 
	}
	a_bIsShutDown = false;

	// TODO MLogManager setup

	a_bIsSetUp = true;
	Success(0,"[MLogmanager.SetUp] done.");
}

void MLogManager::ShutDown()
{
	if(a_bIsShutDown) {
		printf(LOG_COLOR_WARNING "_Warning: [MLogManager.ShutDown] already shutdown.\n" LOG_COLOR_RESET);
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
	printf(LOG_COLOR_SUCCESS "_Success: [MLogManager.ShutDown] done.\n" LOG_COLOR_RESET);
}


void MLogManager::SetLogFile(std::string sFilePath)
{
	SetLogFile(sFilePath,LOG_ANY);
}

void MLogManager::SetLogFile(std::string sFilePath, unsigned int eType)
{
	if(LOG_ERROR & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_ERROR); }
	if(LOG_SUCCESS & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_SUCCESS); }
	if(LOG_INFO & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_INFO); }
	if(LOG_WARNING & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_WARNING); }
	if(LOG_DEBUG & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_DEBUG); }
	if(LOG_LOG & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_LOG); }
	if(LOG_TEST & eType) { Fx_SetLogFile(sFilePath, LOG_TYPEID_TEST); }
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

void MLogManager::Fx_PrintLog(std::string sString, unsigned int iType)
{
	std::string sTypeColor = LOG_COLOR_RESET;

	switch (iType) {
		case LOG_TYPEID_ERROR: sTypeColor = LOG_COLOR_LOG; break;
		case LOG_TYPEID_SUCCESS: sTypeColor = LOG_COLOR_SUCCESS; break;
		case LOG_TYPEID_INFO: sTypeColor = LOG_COLOR_INFO; break;
		case LOG_TYPEID_WARNING: sTypeColor = LOG_COLOR_WARNING; break;
		case LOG_TYPEID_DEBUG: sTypeColor = LOG_COLOR_DEBUG; break;
		case LOG_TYPEID_LOG: sTypeColor = LOG_COLOR_LOG; break;
		case LOG_TYPEID_TEST: sTypeColor = LOG_COLOR_TEST; break;
	}

	printf("%s%s\n" LOG_COLOR_RESET, sTypeColor.c_str(), sString.c_str());
}

bool MLogManager::IsPrint(unsigned int eType)
{	
	bool bAllIsPrint = true;
	
	if(LOG_ERROR & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_ERROR]; }
	if(LOG_SUCCESS & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_SUCCESS]; }
	if(LOG_INFO & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_INFO]; }
	if(LOG_WARNING & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_WARNING]; }
	if(LOG_DEBUG & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_DEBUG]; }
	if(LOG_LOG & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_LOG]; }
	if(LOG_TEST & eType) { bAllIsPrint &= a_sTypeIsPrint[LOG_TYPEID_TEST]; }
	
	return bAllIsPrint;
}

bool MLogManager::IsPrint(bool bIsPrint, unsigned int eType)
{
	if(LOG_ERROR & eType) { a_sTypeIsPrint[LOG_TYPEID_ERROR] = bIsPrint;}
	if(LOG_SUCCESS & eType) { a_sTypeIsPrint[LOG_TYPEID_SUCCESS] = bIsPrint; }
	if(LOG_INFO & eType) { a_sTypeIsPrint[LOG_TYPEID_INFO] = bIsPrint; }
	if(LOG_WARNING & eType) { a_sTypeIsPrint[LOG_TYPEID_WARNING] = bIsPrint; }
	if(LOG_DEBUG & eType) { a_sTypeIsPrint[LOG_TYPEID_DEBUG] = bIsPrint; }
	if(LOG_LOG & eType) { a_sTypeIsPrint[LOG_TYPEID_LOG] = bIsPrint; }
	if(LOG_TEST & eType) { a_sTypeIsPrint[LOG_TYPEID_TEST] = bIsPrint; }

	return bIsPrint;
}
	

/* Log Functions */
void MLogManager::Log(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer,sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_LOG, iCode, LOG_LOG_STR, sBuffer);
}

void MLogManager::Info(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_INFO, iCode, LOG_INFO_STR, sBuffer);
}

void MLogManager::Success(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_SUCCESS, iCode, LOG_SUCCESS_STR, sBuffer);
}

void MLogManager::Warning(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_WARNING, iCode, LOG_WARNING_STR, sBuffer);
}

void MLogManager::Error(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_ERROR, iCode, LOG_ERROR_STR, sBuffer);

	Main::ErrorShutDown();
}

void MLogManager::Debug(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_DEBUG, iCode, LOG_DEBUG_STR, sBuffer);
}

void MLogManager::Test(unsigned int iCode, const char* sFormat, ...)
{
	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	Fx_Log(LOG_TYPEID_TEST, iCode, LOG_TEST_STR, sBuffer);
}

void MLogManager::Any(unsigned int eType, unsigned int iCode, const char* sFormat, ...)
{
	if(!(LOG_ANY & eType)) { Warning(0,"[MLogManager.Any] invalid enum type on evaluation."); return; }

	char sBuffer[LOG_2048_LENGTH];
	va_list vaArgs;
	va_start(vaArgs, sFormat);
	vsprintf_s(sBuffer, sFormat, vaArgs);
	va_end(vaArgs);

	if (LOG_ERROR & eType) { Fx_Log(LOG_TYPEID_ERROR, iCode, LOG_ERROR_STR, sBuffer); }
	if (LOG_SUCCESS & eType) { Fx_Log(LOG_TYPEID_SUCCESS, iCode, LOG_SUCCESS_STR, sBuffer); }
	if (LOG_INFO & eType) { Fx_Log(LOG_TYPEID_INFO, iCode, LOG_INFO_STR, sBuffer); }
	if (LOG_WARNING & eType) { Fx_Log(LOG_TYPEID_WARNING, iCode, LOG_WARNING_STR, sBuffer); }
	if (LOG_DEBUG & eType) { Fx_Log(LOG_TYPEID_DEBUG, iCode, LOG_DEBUG_STR, sBuffer); }
	if (LOG_LOG & eType) { Fx_Log(LOG_TYPEID_LOG, iCode, LOG_LOG_STR, sBuffer); }
	if (LOG_TEST & eType) { Fx_Log(LOG_TYPEID_TEST, iCode, LOG_TEST_STR, sBuffer); }
}

void MLogManager::Fx_Log(unsigned int iType, unsigned int iCode, char sPrefix[], char sMessage[])
{
	if (a_bIsShutDown) {
		printf(LOG_COLOR_WARNING "_Warning: [MLogManager.Fx_Log] allready shutdown.\n" LOG_COLOR_RESET);
		return;
	}

	++a_iLogCounter;

	std::string sString = sPrefix + ( iCode ? "(" + std::to_string(iCode) + "): " : ": " ) + sMessage;
	
	Fx_WriteLog(sString, iType);
	if (a_sTypeIsPrint[iType]) { Fx_PrintLog(sString,iType);}
}
