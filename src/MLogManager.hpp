#ifndef MLOGMANAGER_HPP
#define MLOGMANAGER_HPP
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <fstream>

#define OGLFW_DEBUG_ZERO 0
#define OGLFW_DEBUG_ONE 1

#define LOG_2_LENGTH 2
#define LOG_4_LENGTH 4
#define LOG_8_LENGTH 8
#define LOG_16_LENGTH 16
#define LOG_32_LENGTH 32
#define LOG_64_LENGTH 64
#define LOG_128_LENGTH 128
#define LOG_256_LENGTH 256
#define LOG_512_LENGTH 512
#define LOG_1024_LENGTH 1024
#define LOG_2048_LENGTH 2048

#define LOG_LOG_STR "Log."

#define LOG_ERROR_STR "Error:"
#define LOG_SUCCESS_STR "Success:"
#define LOG_INFO_STR "Info:"
#define LOG_WARNING_STR "Warning:"
#define LOG_DEBUG_STR "Debug:"

#define LOG_COLOR_RESET "\u001b[0m"

#define LOG_COLOR_ERROR "\u001b[31m"
#define LOG_COLOR_SUCCESS "\u001b[32m"
#define LOG_COLOR_INFO "\u001b[36m"/*BLUE "\u001b[34m"*/
#define LOG_COLOR_WARNING "\u001b[33m"
#define LOG_COLOR_DEBUG LOG_COLOR_RESET

#define LOG_WARNING_USLEEP_TIME (2*1e+6)

#define LOG_FILE_FLUSHRATE 32


#define LOG_TYPE_COUNT 5
enum ELogType {
	LOG_ERROR = 1<<0,
	LOG_SUCCESS = 1<<1,
	LOG_INFO = 1<<2,
	LOG_WARNING = 1<<3,
	LOG_DEBUG = 1<<4,
	
	LOG_DEFAULT = LOG_DEBUG,
	LOG_ANY = LOG_ERROR | LOG_SUCCESS | LOG_INFO | LOG_WARNING | LOG_DEBUG,
	LOG_ALL = ~0
};

class MLogManager;
class MLogManager
{
private:
	MLogManager();
	static MLogManager* a_oInstance;
	bool a_bIsSetUp;
	bool a_bIsShutDown;

	long unsigned int a_iLogCounter; 	

	std::string a_lTypeFilePaths[LOG_TYPE_COUNT];
	std::ofstream a_lFileStreames[LOG_TYPE_COUNT];
	bool a_sTypeIsPrint[LOG_TYPE_COUNT];

	void Fx_WriteLog(std::string sString, unsigned int iFile);
	void Fx_SetLogFile(std::string sFilePath, unsigned int iFile);
	void Fx_PrintLog(std::string sString);
	void Fx_EvaluateLog(std::string sLog, unsigned int iCode, unsigned int eType);
public:
	~MLogManager();
	static MLogManager* GetInstance();
	void SetUp();
	void ShutDown();

	void SetLogFile(std::string sFilePath);
	void SetLogFile(std::string sFilePath, unsigned int eType);

	bool IsPrint(unsigned int eType);
	bool IsPrint(bool bIsPrint, unsigned int eType);
	
	/* Log Functions */
	void LogF(unsigned int eType, unsigned int iCode, const char* sFormat, ...);
	void Log(std::string sMessege);
	void Log(std::string sMessege, unsigned int iCode);
	void Log(std::string sMessege, unsigned int iCode, unsigned int eType);

	void Log(std::string sMessege, std::string sString, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, char* sString, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, char cChar, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, unsigned char cChar, unsigned int iCode, unsigned int eType);

	void Log(std::string sMessege, unsigned int iNumber, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, int iNumber, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, long iNumber, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, double dNumber, unsigned int iCode, unsigned int eType);
	void Log(std::string sMessege, float dNumber, unsigned int iCode, unsigned int eType);
};
#endif
