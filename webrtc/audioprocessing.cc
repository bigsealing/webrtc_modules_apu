#include "audioprocessing.h"
#include "modules/include/module_common_types.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "api/audio/audio_frame.h"

somo::video::IAudioProcessing* somo::video::AudioProcessingFactory::createAudioProcessing(){
    somo::video::IAudioProcessing* opt = new AudioProcessing();
    return  opt;
}

void somo::video::AudioProcessingFactory::releaseAudioProcessing(somo::video::IAudioProcessing* opt)
{
    delete opt;
    opt = NULL;
}


static int k_debugseq = 0;
AudioProcessing::AudioProcessing()
: m_nSamplesPerChannel(0)
, m_nStreamDelay(0)
, m_nAEC_driftSamples(0)
, m_nAGC_analogLevel(0)
, m_pAudioProc(NULL)
{
}

AudioProcessing::~AudioProcessing()
{
    if( m_pAudioProc )
        delete m_pAudioProc;
    if( m_pConfig )
        delete m_pConfig;
    if( m_pInputConfig )
        delete m_pInputConfig;
    if( m_pOutputConfig )
        delete m_pOutputConfig;
    if( m_pReverseConfig )
        delete m_pReverseConfig;
}

void AudioProcessing::setStreamDelay(int delay)
{
    m_nStreamDelay = delay;
}


bool AudioProcessing::init(int in_samplerate, int in_channelnum, int out_samplerate, int out_channelnum)
{
	m_nSamplesPerChannel = in_samplerate / 100; //AudioProcessModel(APM) only handle 10ms & 16bit PCM data!!!

    m_pAudioProc = webrtc::AudioProcessingBuilder().Create();
    enableHPF(true);
    enableAEC(true); //enable AEC will disable AECM
    enableECM(false); //enable AECM will disable AEC
    enableNS(true);
    enableVAD(true);
    enableLE(true);
    enableAGC(false);

    m_pInputConfig	= new webrtc::StreamConfig(in_samplerate, in_channelnum);
    m_pOutputConfig	= new webrtc::StreamConfig(out_samplerate, out_channelnum);
    m_pReverseConfig	= new webrtc::StreamConfig(in_samplerate, in_channelnum);
    const webrtc::ProcessingConfig _config = {{*m_pInputConfig, *m_pOutputConfig, *m_pReverseConfig, *m_pReverseConfig}};
    if ( webrtc::AudioProcessing::kNoError != m_pAudioProc->Initialize(_config) )
    {
        printf("AudioProcessing Initialize fail!");
        return false;
    }
    m_pAudioProc->ApplyConfig(m_apmConfig);
    printf("AudioProcessing Initialize success, in_samplerate=%d out_samplerate=%d in_channelnum=%d out_channelnum=%d", in_samplerate, out_samplerate, in_channelnum, out_channelnum);
	return true;
}

void AudioProcessing::processCapture(char* inBuf, int inLen, bool& o_VADLowPower)
{
    o_VADLowPower = true; //VAD detect voice
	int frameLen = m_nSamplesPerChannel * 2 * m_pInputConfig->num_channels();//16bits, 2bytes
	int frameNum = inLen / frameLen; //TO DO: make sure
	char* cur = (char*)inBuf;
	int ret = 0;

//    bool isAECDriftCompensationEnable = m_pAudioProc->echo_cancellation()->is_drift_compensation_enabled();
    bool isVADEnable = m_apmConfig.voice_detection.enabled;
//    bool isAGCAnalogModEnable = m_pAudioProc->gain_control()->is_enabled() && m_pAudioProc->gain_control()->mode() == webrtc::GainControl::kAdaptiveDigital;
	for ( int i=0; i<frameNum; i++ )
	{
		webrtc::AudioFrame frame;
        frame.sample_rate_hz_ = m_pInputConfig->sample_rate_hz();
		frame.samples_per_channel_ = m_nSamplesPerChannel;
        frame.num_channels_ = m_pInputConfig->num_channels();
        frame.vad_activity_ = webrtc::AudioFrame::kVadUnknown;
        memcpy(frame.mutable_data(), cur, frameLen);

		m_pAudioProc->set_stream_delay_ms(m_nStreamDelay);
//        if ( isAECDriftCompensationEnable )
//            m_pAudioProc->echo_cancellation()->set_stream_drift_samples(m_nAEC_driftSamples);
//        if ( isAGCAnalogModEnable )
//            m_pAudioProc->gain_control()->set_stream_analog_level(m_nAGC_analogLevel);
        ret = m_pAudioProc->ProcessStream(&frame);
		if ( ret != webrtc::AudioProcessing::kNoError )
		{
            printf("ProcessStream error! errcode=%d", ret);
            cur += frameLen;
			continue;
		}
        m_nStreamDelay = m_pAudioProc->stream_delay_ms();
//        if ( isAECDriftCompensationEnable )
//            m_nAEC_driftSamples = m_pAudioProc->echo_cancellation()->stream_drift_samples();
//        if ( isAGCAnalogModEnable )
//            m_nAGC_analogLevel = m_pAudioProc->gain_control()->stream_analog_level();

		/*
        if ( !isVADEnable || m_pAudioProc->voice_detection()->stream_has_voice() )
		{
			memcpy(cur, frame.data_, frameLen);
		}
		else
		{
			memset(cur, 0, frameLen);
		}
        */
        memcpy(cur, frame.data(), frameLen);
//        o_VADLowPower &= (isVADEnable && !m_pAudioProc->stream_has_voice());
        //printf("Debug:%d hasVoice10ms=%d lowPower=%d speech=%f hasEcho=%d", k_debugseq, m_pAudioProc->voice_detection()->stream_has_voice(), o_VADLowPower, m_pAudioProc->noise_suppression()->speech_probability(), m_pAudioProc->echo_cancellation()->stream_has_echo());

		cur += frameLen;
	}
	++k_debugseq;
}

void AudioProcessing::processRemote(char* inBuf, int inLen)
{
	int frameLen = m_nSamplesPerChannel * 2 * m_pReverseConfig->num_channels(); //16bits, 2bytes
	int frames = inLen/ frameLen;
	char* cur = (char*)inBuf;

	for ( int i=0; i<frames; i++ )
	{
		webrtc::AudioFrame frame;
        frame.sample_rate_hz_ = m_pReverseConfig->sample_rate_hz();
		frame.samples_per_channel_ = m_nSamplesPerChannel;
        frame.num_channels_ = m_pReverseConfig->num_channels();
		frame.vad_activity_ = webrtc::AudioFrame::kVadUnknown;
        memcpy(frame.mutable_data(), cur, frameLen);

        int errcode = m_pAudioProc->ProcessReverseStream(&frame);
        if ( errcode != webrtc::AudioProcessing::kNoError )
        {
            printf("ProcessReverseStream error! errcode=%d", errcode);
            cur += frameLen;
            continue;
        }
        memcpy(cur, frame.data(), frameLen);
        cur += frameLen;
	}
}

void AudioProcessing::enableAGC(bool enable)
{
    if (enable != m_apmConfig.gain_controller2.enabled) {
        m_apmConfig.gain_controller2.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableNS(bool enable)
{
    if (enable != m_apmConfig.noise_suppression.enabled){
        m_apmConfig.noise_suppression.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableVAD(bool enable)
{
    if (enable != m_apmConfig.voice_detection.enabled){
        m_apmConfig.voice_detection.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableECM(bool enable)
{
    if (enable != m_apmConfig.echo_canceller.mobile_mode){
        m_apmConfig.echo_canceller.enabled = false;
        m_apmConfig.echo_canceller.mobile_mode = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableAEC(bool enable)
{
    if (enable != m_apmConfig.echo_canceller.enabled) {
        m_apmConfig.echo_canceller.enabled = enable;
        m_apmConfig.echo_canceller.mobile_mode = false;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableHPF(bool enable)
{
    printf("%s high pass fileter", enable? "enable" : "disable");
    if(enable != m_apmConfig.high_pass_filter.enabled){
        m_apmConfig.high_pass_filter.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioProcessing::enableLE(bool enable)
{
    if (enable != m_apmConfig.level_estimation.enabled) {
        m_apmConfig.level_estimation.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}
