
#include"real_sence.hpp"

#include<filesystem>

#include"log.hpp"
#include"utils.hpp"

namespace Prizm
{
	RealSence::RealSence()
		: instance_(nullptr)
		, audio_source_(nullptr)
		, speech_recog_(nullptr)
		, handler_(nullptr)
	{}

	RealSence::~RealSence(){}

	bool RealSence::initialize(void)
	{
		// realsence initialize
		instance_ = PXCSession::CreateInstance();

		if (!instance_)
		{
			Log::error("Unable to create pxcsession instance.");
			finalize();
			return false;
		}

		enumAudioSourceDeviceInfo();

		return true;
	}

	void RealSence::finalize(void)
	{
		if (handler_)
			handler_ = nullptr;

		if (speech_recog_)
		{
			speech_recog_->Release();
			speech_recog_ = nullptr;
		}

		if (audio_source_)
		{
			audio_source_->Release();
			audio_source_ = nullptr;
		}

		if (instance_)
		{
			instance_->Release();
			instance_ = nullptr;
		}
	}

	bool RealSence::startRec(void)
	{
		audio_source_ = instance_->CreateAudioSource();

		if (!audio_source_)
		{
			Log::error("Unable to create audio source.");
			audio_source_->Release();
			return false;
		}

		PXCAudioSource::DeviceInfo device_info = device_infoes_[0];

		if (audio_source_->SetDevice(&device_info))
		{
			Log::error("Unable to set device info.");
			audio_source_->Release();
			return false;
		}

		// speech recognizer initialize
		if (instance_->CreateImpl<PXCSpeechRecognition>(&speech_recog_) != pxcStatus::PXC_STATUS_NO_ERROR)
		{
			Log::error("Unable to create speech recognizer.");
			audio_source_->Release();
			return false;
		}

		// initialize the Module
		PXCSpeechRecognition::ProfileInfo info;
		speech_recog_->QueryProfile(0, &info);
		if (speech_recog_->SetProfile(&info) != pxcStatus::PXC_STATUS_NO_ERROR)
		{
			Log::error("Unable to set profile.");
			speech_recog_->Release();
			return false;
		}

		// set the recognition mode
		// command and control mode or dictation mode
		if (speech_recog_->SetDictation() != pxcStatus::PXC_STATUS_NO_ERROR)
		{
			Log::error("Unable to set dictation.");
			speech_recog_->Release();
			return false;
		}

		// set message handler and start recognition 
		handler_ = new RecognitionHandler;
		if (speech_recog_->StartRec(nullptr, handler_) != pxcStatus::PXC_STATUS_NO_ERROR)
		{
			Log::error("Unable to start recognition.");
			speech_recog_->Release();
			return false;
		}

		Log::info("Start recognition.");

		return true;
	}

	void RealSence::stopRec(void)
	{
		speech_recog_->StopRec();

		Log::info("Stop recognition.");

		audio_source_->Release();
		audio_source_ = nullptr;
	}

	void RealSence::enumAudioSourceDeviceInfo(void)
	{
		PXCAudioSource* source = instance_->CreateAudioSource();

		if (source)
		{
			source->ScanDevices();

			for (int i = 0;; ++i)
			{
				PXCAudioSource::DeviceInfo device_info = {};
				if (source->QueryDeviceInfo(i, &device_info) < PXC_STATUS_NO_ERROR) break;
				device_infoes_[i] = device_info;
				Log::info(StrUtils::toString(device_infoes_[i].name));
			}
		}
	}
}