#include "webrtctestfile.h"

//#define ENABLE_WEBRTC_TEST_FILE

WebRTCTestFile::WebRTCTestFile(std::string filename)
    :m_pFile(NULL)
{
#ifdef IOS
    //std::string path = ProtoMyInfo::instance()->getIOSSandboxPath();
    //filename = path.append("/").append(filename);
    //FUNLOGI("ios file path = %s", filename.c_str());
#else if ANDROID
    //std::string audioPath = ProtoMyInfo::instance()->getWebRTCSaveAudioPath();
    //if(!audioPath.empty()){
    //    filename = audioPath.append("/").append(filename);
    //    FUNLOGI("android file path = %s", filename.c_str());
    //}
#endif
    
#ifdef ENABLE_WEBRTC_TEST_FILE
    FUNLOGW("create test file:%s", filename.c_str());
    m_filename = filename;
    m_pFile = fopen(filename.c_str(), "wb+");
    if (m_pFile == NULL){
        FUNLOGE("file create failed. name = %s", filename.c_str());
    }
#endif
}

WebRTCTestFile::~WebRTCTestFile()
{
    closeFile();
}

int WebRTCTestFile::write(const char* data, int len)
{
    if (m_pFile) {
        return fwrite(data, 1, len, m_pFile);
    }
	return 0;
}

void WebRTCTestFile::closeFile()
{
    if (m_pFile){
        fclose(m_pFile);
        m_pFile = NULL;
    }
}

int WebRTCTestFile::removeFile()
{
    if (m_pFile) {
        closeFile();
        return remove(m_filename.c_str());
    }
    return -1;
}

void WebRTCTestFile::setFileName(std::string fileName) {
    //FUNLOGW("create test file:%s", fileName.c_str());
    m_filename = fileName;
    closeFile();
    m_pFile = fopen(fileName.c_str(), "wb+");
    if (m_pFile == NULL){
        //FUNLOGE("file create failed. name = %s", fileName.c_str());
    }
}

bool WebRTCTestFile::isHadFile() {
    if(m_pFile){
        return true;
    }
    return false;
}

