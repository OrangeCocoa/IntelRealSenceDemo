#pragma once

#include<memory>
#include<unordered_map>
#include<filesystem>

#include<pxcsensemanager.h>
#include<pxcspeechrecognition.h>
#include<pxcspeechsynthesis.h>

#include"log.hpp"

namespace Prizm
{
	// message handler
	class RecognitionHandler : public PXCSpeechRecognition::Handler
	{
	private:
		const char* AlertToString(PXCSpeechRecognition::AlertType label)
		{
			switch (label) {
			case PXCSpeechRecognition::ALERT_SNR_LOW: return "SNR_LOW";
			case PXCSpeechRecognition::ALERT_SPEECH_UNRECOGNIZABLE: return "SPEECH_UNRECOGNIZABLE";
			case PXCSpeechRecognition::ALERT_VOLUME_HIGH: return "VOLUME_HIGH";
			case PXCSpeechRecognition::ALERT_VOLUME_LOW: return "VOLUME_LOW";
			case PXCSpeechRecognition::ALERT_SPEECH_BEGIN: return "SPEECH_BEGIN";
			case PXCSpeechRecognition::ALERT_SPEECH_END: return "SPEECH_END";
			case PXCSpeechRecognition::ALERT_RECOGNITION_ABORTED: return "RECOGNITION_ABORTED";
			case PXCSpeechRecognition::ALERT_RECOGNITION_END: return "RECOGNITION_END";
			}
			return "Unknown";
		}

	public:
		virtual void __stdcall OnRecognition(const PXCSpeechRecognition::RecognitionData *data)override
		{
			std::filesystem::path sentence(data->scores[0].sentence);
			Log::info("Output : " + sentence.string());
		}

		virtual void __stdcall OnAlert(const PXCSpeechRecognition::AlertData *data)override
		{
			Log::info(AlertToString(data->label));			
		}
	};

	class RealSence
	{
	private:
		PXCSession*           instance_;

		// PXCAudioSource is audio endpoint?
		PXCAudioSource*       audio_source_;

		PXCSpeechRecognition* speech_recog_;
		RecognitionHandler*   handler_;

		std::unordered_map<int, PXCAudioSource::DeviceInfo> device_infoes_;

	public:
		RealSence();
		~RealSence();

		bool initialize(void);
		void finalize(void);

		// recognition
		bool startRec(void);
		void stopRec(void);

		void enumAudioSourceDeviceInfo(void);
	};
}