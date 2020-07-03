#pragma once

#ifdef _WIN32
#define WEBRTC_AUDIO_OPTIMIZE_API __declspec(dllexport)
#else
#define WEBRTC_AUDIO_OPTIMIZE_API
#endif

namespace somo {
namespace video {
struct IAudioOptimize
{
public:
    virtual bool init(int in_samplerate, int in_channelnum, int out_samplerate, int out_channelnum) = 0;
    virtual void processCapture(char* inBuf, int inLen, bool& o_VADLowPower) = 0; //MUST be 16bit PCM data!
    virtual void processRemote(char* inBuf, int inLen) = 0; //MUST be 16bit PCM data!

    virtual void setStreamDelay(int delay) = 0;

    virtual void enableAGC(bool enable) = 0;
    virtual void enableNS(bool enable) = 0;
    virtual void enableVAD(bool enable) = 0;
    virtual void enableECM(bool enable) = 0;
    virtual void enableAEC(bool enable) = 0;
    virtual void enableHPF(bool enable) = 0;
    virtual void enableLE(bool enable) = 0;
};

class WEBRTC_AUDIO_OPTIMIZE_API AudioOptimizeFactory
{
public:
    static IAudioOptimize* createAudioOptimize();
    static void releaseAudioOptimize(IAudioOptimize* opt);
};

}
}
