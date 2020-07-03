#ifndef FILEWRITEHELPER_H
#define FILEWRITEHELPER_H

#include <stdio.h>
#include <string>

//NOTE:this class is not thread-safe
class WebRTCTestFile
{
public:
    WebRTCTestFile(std::string filename);
    ~WebRTCTestFile();

    int write(const char* data, int len);
    void closeFile();
    int removeFile();
    void setFileName(std::string fileName);
    bool isHadFile();

private:
    FILE* m_pFile;
    std::string m_filename;
};

#endif // FILEWRITEHELPER_H
