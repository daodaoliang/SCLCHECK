//File: ScdIcdChecker.h 
//gsoap ns service name: ScdIcdChecker
//gsoap ns service namespace: urn:ScdIcdChecker
//gsoap ns service location: http://192.168.1.62:23389/

// strResultJson为utf8编码的json字符串，格式：
// {
//     "status" : "success" 或 "failed",
//     "message" : "some reason"
// }

// scd检测
int ns__ScdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);

// icd检测
int ns__IcdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);

// scd中的ied的crc计算
int ns__ScdCrcCalc(std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);
