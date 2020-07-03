#include "audiooptimize.h"
#include "logsdk/logger.h"
//#include "core/macroformat.h"
#include "modules/include/module_common_types.h"
#include "modules/audio_processing/include/audio_processing.h"
#include "api/audio/audio_frame.h"

static int k_debugseq = 0;

somo::video::IAudioOptimize* somo::video::AudioOptimizeFactory::createAudioOptimize(){
    somo::video::IAudioOptimize* opt = new AudioOptimize();
    return  opt;
}

void somo::video::AudioOptimizeFactory::releaseAudioOptimize(somo::video::IAudioOptimize* opt)
{
    delete opt;
    opt = NULL;
}

AudioOptimize::AudioOptimize()
: m_nSamplesPerChannel(0)
, m_nStreamDelay(0)
, m_nAEC_driftSamples(0)
, m_nAGC_analogLevel(0)
, m_pAudioProc(NULL)
{
//    m_pConfig = new webrtc::Config();
//    m_pConfig->Set<webrtc::Intelligibility>(new webrtc::Intelligibility(true)); //enable intelligibility
//    cfg.Set<webrtc::Beamforming>(new webrtc::Beamfom_pMutexrming(true, geometryArray, webrtc::SphericalPointf(webrtc::DegreesToRadians(90), 0.f, 1.f))); //enable Beamforming
//    m_pConfig->Set<webrtc::ExperimentalNs>(new webrtc::ExperimentalNs(true));
//    m_pConfig->Set<webrtc::LevelControl>(new webrtc::LevelControl(true));
//    m_pAudioProc = webrtc::AudioProcessing::Create(*m_pConfig);
}

AudioOptimize::~AudioOptimize()
{
	//webrtc::AudioProcessing::Destroy(m_pAudioProc);
    FUNLOGI(" dealloc");
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
	//SAFE_DELETE(m_pAudioProc);
    //SAFE_DELETE(m_pConfig);
    //SAFE_DELETE(m_pInputConfig);
    //SAFE_DELETE(m_pOutputConfig);
    //SAFE_DELETE(m_pReverseConfig);
}

void AudioOptimize::setStreamDelay(int delay)
{
    FUNLOGI("setStreamDelay, delay = %d", delay);
    m_nStreamDelay = delay;
}


bool AudioOptimize::init(int in_samplerate, int in_channelnum, int out_samplerate, int out_channelnum)
{
	m_nSamplesPerChannel = in_samplerate / 100; //AudioProcessModel(APM) only handle 10ms & 16bit PCM data!!!

    m_pAudioProc = webrtc::AudioProcessingBuilder().Create();

//    m_apmConfig.echo_canceller.enabled = true;
//    m_apmConfig.echo_canceller.mobile_mode = false;

//    m_apmConfig.gain_controller1.enabled = true;
//    m_apmConfig.gain_controller1.mode =
//            webrtc::AudioProcessing::Config::GainController1::kAdaptiveAnalog;
//    m_apmConfig.gain_controller1.analog_level_minimum = 0;
//    m_apmConfig.gain_controller1.analog_level_maximum = 255;

//    m_apmConfig.gain_controller2.enabled = true;
//    m_apmConfig.high_pass_filter.enabled = true;
//    m_apmConfig.voice_detection.enabled = true;

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
        FUNLOGW("AudioProcessing Initialize fail!");
        return false;
    }
    m_pAudioProc->ApplyConfig(m_apmConfig);
	FUNLOGI("AudioProcessing Initialize success, in_samplerate=%d out_samplerate=%d in_channelnum=%d out_channelnum=%d", in_samplerate, out_samplerate, in_channelnum, out_channelnum);
	return true;
}

void AudioOptimize::processCapture(char* inBuf, int inLen, bool& o_VADLowPower)
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
			FUNLOGW("ProcessStream error! errcode=%d", ret);
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
        //FUNLOGI("Debug:%d hasVoice10ms=%d lowPower=%d speech=%f hasEcho=%d", k_debugseq, m_pAudioProc->voice_detection()->stream_has_voice(), o_VADLowPower, m_pAudioProc->noise_suppression()->speech_probability(), m_pAudioProc->echo_cancellation()->stream_has_echo());

		cur += frameLen;
	}
	++k_debugseq;
}

void AudioOptimize::processRemote(char* inBuf, int inLen)
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
            FUNLOGW("ProcessReverseStream error! errcode=%d", errcode);
            cur += frameLen;
            continue;
        }
        memcpy(cur, frame.data(), frameLen);
        cur += frameLen;
	}
}

void AudioOptimize::enableAGC(bool enable)
{
//	if (enable)
//	{
//		FUNLOGI(" enable automatic gain control, mode=kAdaptiveAnalog");
//        m_pAudioProc->gain_control()->set_mode(webrtc::GainControl::kAdaptiveDigital);
//        m_pAudioProc->gain_control()->set_analog_level_limits(0, 255);
////        m_pAudioProc->gain_control()->set_target_level_dbfs(11);
//	}
//	m_pAudioProc->gain_control()->Enable(enable);
    if (enable != m_apmConfig.gain_controller2.enabled) {
        m_apmConfig.gain_controller2.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableNS(bool enable)
{
//	if (enable)
//	{
//        FUNLOGI(" enable noise suppression, level=kHigh");
//        m_pConfig->Set<webrtc::Intelligibility>(new webrtc::Intelligibility(true));
//        m_pAudioProc->noise_suppression()->set_level(webrtc::NoiseSuppression::kHigh);
//	}
//    else
//    {
//        //if not add the following line, enable == NO, will crash
//        m_pConfig->Set<webrtc::Intelligibility>(new webrtc::Intelligibility(false));
//    }
//    m_pAudioProc->SetExtraOptions(*m_pConfig);
//	m_pAudioProc->noise_suppression()->Enable(enable);
    if (enable != m_apmConfig.noise_suppression.enabled){
        m_apmConfig.noise_suppression.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableVAD(bool enable)
{
//	if (enable)
//	{
//        if ( m_pAudioProc->echo_cancellation()->is_enabled() )
//        {
//            FUNLOGI(" enable voice activity detection(AEC already enabled), likelihood=kModerateLikelihood");
//            m_pAudioProc->voice_detection()->set_likelihood(webrtc::VoiceDetection::kModerateLikelihood);
//        }
//        else
//        {
//            FUNLOGI(" enable voice activity detection, likelihood=kModerateLikelihood");
//            m_pAudioProc->voice_detection()->set_likelihood(webrtc::VoiceDetection::kModerateLikelihood);
//        }
//	}
//	m_pAudioProc->voice_detection()->Enable(enable);
    if (enable != m_apmConfig.voice_detection.enabled){
        m_apmConfig.voice_detection.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableECM(bool enable)
{
//	if (enable)
//	{
//        FUNLOGI(" enable echo control mobile, disable comfort_noise, routing_mode=kLoudSpeakerphone");
//        m_pAudioProc->echo_control_mobile()->enable_comfort_noise(false);
//        m_pAudioProc->echo_control_mobile()->set_routing_mode(webrtc::EchoControlMobile::kLoudSpeakerphone);
//	}
//	m_pAudioProc->echo_control_mobile()->Enable(enable);
    if (enable != m_apmConfig.echo_canceller.mobile_mode){
        m_apmConfig.echo_canceller.enabled = false;
        m_apmConfig.echo_canceller.mobile_mode = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableAEC(bool enable)
{
	if (enable)
	{
//        FUNLOGI(" enable acoustic echo cancellation, suppression_level=kVeryHighSuppression");
//#if defined(__ANDROID__) || defined(ANDROID)
//        m_pAudioProc->echo_cancellation()->set_suppression_level(webrtc::EchoCancellation::kVeryHighSuppression);
//#else
//        m_pAudioProc->echo_cancellation()->set_suppression_level(webrtc::EchoCancellation::kSavageSuppression);
//#endif
//        //m_pAudioProc->echo_cancellation()->enable_drift_compensation(true);
//        //m_pAudioProc->echo_cancellation()->enable_metrics(true);

//        m_pConfig->Set<webrtc::ExtendedFilter>(new webrtc::ExtendedFilter(true)); //enable the extended filter in the AEC, will increase complexity
//#if !defined(__ANDROID__) && !defined(ANDROID)
//        FUNLOGI(" enable acoustic echo cancellation, aec v3");
//        m_pConfig->Set<webrtc::EchoCanceller3>(new webrtc::EchoCanceller3(true)); //AEC V3
//        m_pConfig->Set<webrtc::RefinedAdaptiveFilter>(new webrtc::RefinedAdaptiveFilter(true)); //AEC refined
//#endif
//        m_pConfig->Set<webrtc::DelayAgnostic>(new webrtc::DelayAgnostic(true)); //olny work for AEC, NOT for AECM, can be set by SetExtraOptions
//        m_pAudioProc->SetExtraOptions(*m_pConfig);
	}
    if (enable != m_apmConfig.echo_canceller.enabled) {
        m_apmConfig.echo_canceller.enabled = enable;
        m_apmConfig.echo_canceller.mobile_mode = false;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableHPF(bool enable)
{
	FUNLOGI("%s high pass fileter", enable? "enable" : "disable");
    if(enable != m_apmConfig.high_pass_filter.enabled){
        m_apmConfig.high_pass_filter.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}

void AudioOptimize::enableLE(bool enable)
{
    if (enable != m_apmConfig.level_estimation.enabled) {
        m_apmConfig.level_estimation.enabled = enable;
        m_pAudioProc->ApplyConfig(m_apmConfig);
    }
}
