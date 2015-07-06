#ifndef MLOGMANAGER_HPP
#define MLOGMANAGER_HPP
#include <cstdarg>
#include <cstdio>
#include <iostream>
#include <fstream>

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

#define LOG_LOG_STR "Log"
#define LOG_ERROR_STR "Error"
#define LOG_SUCCESS_STR "Success"
#define LOG_INFO_STR "Info"
#define LOG_WARNING_STR "Warning"
#define LOG_DEBUG_STR "Debug"
#define LOG_TEST_STR "Test"

#define LOG_COLOR_RESET ""

/*BLUE "\u001b[34m"*/
#define LOG_COLOR_LOG ""
#define LOG_COLOR_ERROR ""
#define LOG_COLOR_SUCCESS ""
#define LOG_COLOR_INFO ""
#define LOG_COLOR_WARNING ""
#define LOG_COLOR_TEST ""
#define LOG_COLOR_DEBUG LOG_COLOR_RESET

#define LOG_WARNING_USLEEP_TIME (2*1e+6)
#define LOG_FILE_FLUSHRATE 32

#define LOG_TYPEID_LOG 0
#define LOG_TYPEID_ERROR 1
#define LOG_TYPEID_SUCCESS 2
#define LOG_TYPEID_INFO 3
#define LOG_TYPEID_WARNING 4
#define LOG_TYPEID_DEBUG 5
#define LOG_TYPEID_TEST 6

#define LOG_TYPE_COUNT 7
enum ELogType {
	LOG_ERROR = 1 << LOG_TYPEID_ERROR,
	LOG_SUCCESS = 1 << LOG_TYPEID_SUCCESS,
	LOG_INFO = 1 << LOG_TYPEID_INFO,
	LOG_WARNING = 1 << LOG_TYPEID_WARNING,
	LOG_DEBUG = 1 << LOG_TYPEID_DEBUG,
	LOG_LOG = 1 << LOG_TYPEID_LOG,
	LOG_TEST = 1 << LOG_TYPEID_TEST,
	
	LOG_DEFAULT = LOG_DEBUG,
	LOG_ANY = LOG_ERROR | LOG_SUCCESS | LOG_INFO | LOG_WARNING | LOG_DEBUG | LOG_LOG | LOG_TEST,
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
	void Fx_PrintLog(std::string sString, unsigned int iType);
	void Fx_Log(unsigned int iType, unsigned int iCode, char sPrefix[], char sMessage[]);
public:
	~MLogManager();
	static MLogManager* GetInstance();
	void SetUp();
	void ShutDown();

	void SetLogFile(std::string sFilePath);
	void SetLogFile(std::string sFilePath, unsigned int eType);

	bool IsPrint(unsigned int eType);
	bool IsPrint(bool bIsPrint, unsigned int eType);

	/*void Flush();*/
	
	/* log functions */
	void Log(unsigned int iCode, const char* sFormat, ...);
	void Info(unsigned int iCode, const char* sFormat, ...);
	void Success(unsigned int iCode, const char* sFormat, ...);
	void Warning(unsigned int iCode, const char* sFormat, ...);
	void Error(unsigned int iCode, const char* sFormat, ...);
	void Debug(unsigned int iCode, const char* sFormat, ...);
	void Test(unsigned int iCode, const char* sFormat, ...);
	void Any(unsigned int eType, unsigned int iCode, const char* sFormat, ...);
};
#endif
