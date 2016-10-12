//File: ScdIcdChecker.h 
//gsoap ns service name: ScdIcdChecker
//gsoap ns service namespace: urn:ScdIcdChecker
//gsoap ns service location: http://192.168.1.62:23389/

// strResultJsonΪutf8�����json�ַ�������ʽ��
// {
//     "status" : "success" �� "failed",
//     "message" : "some reason"
// }

// scd���
int ns__ScdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);

// icd���
int ns__IcdCheck(std::string strConfigFilePath, std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);

// scd�е�ied��crc����
int ns__ScdCrcCalc(std::string strFilePath, std::string strResultStorePath, std::string &strResultJson);
