#ifndef _ASR_FP_MM_AUDIOPOLICY_H_
#define _ASR_FP_MM_AUDIOPOLICY_H_

#include "mci.h"

typedef enum {
	STRATEGY_MEDIA,
	STRATEGY_PHONE,
	STRATEGY_SONIFICATION,
	NUM_STRATEGIES,
} routing_strategy_t;
typedef enum {
	VOLUME_MEDIA,
	VOLUME_PHONE,
	VOLUME_NOTIFICATION,
	NUM_VOLUME_STRATEGIES,
} volume_strategy_t;

class AudioPolicyManager
{
	public:
		static AudioPolicyManager* getInstance() {
			if (AudioPolicy == NULL) {
				AudioPolicy = new AudioPolicyManager;
			}
			return AudioPolicy;
		}

		AudioPolicyManager();
		~AudioPolicyManager();

		class GarbageCollect
		{
			public:
				~GarbageCollect() {
					if (AudioPolicyManager::AudioPolicy != NULL) {
						delete AudioPolicyManager::AudioPolicy;
						AudioPolicy = NULL;
					}
				}
		};

		int setOutputDevice(int stream);
		void setOutputDeviceConectionState(MCI_AUDIO_PATH_T device, device_state_t state);
		int setDeviceForceUse(MCI_AUDIO_PATH_T output);
		int setDeviceMute(MCI_AUDIO_PATH_T output);
		void setCurrentStreamType(audio_stream_type_t type);
		void clearCurrentStreamType();
		int isInCall();
		int isVoiceStream();
		int isInCallState(audio_mode_t mode);
		int setPhoneState(audio_mode_t mode);
		int getPhoneState();
		int updateOutdevice(MCI_AUDIO_PATH_T device);
		routing_strategy_t getStrategyForStream(audio_stream_type_t stream);
		MCI_AUDIO_PATH_T getDeviceForStrategy(routing_strategy_t strategy);
		int getRoute();
		void setAudioVolume(unsigned int volume, volume_strategy_t vol_strategy);
		unsigned int getAudioVolume(volume_strategy_t vol_strategy);		
		int mAvailableOutputDevices;
		int mcurrentOutputDevice;
		audio_stream_type_t mCurrentStreamType;
		audio_mode_t mCurrentPhoneState;
		unsigned int mAudioTypeVolume[NUM_VOLUME_STRATEGIES];
	private:
		static GarbageCollect Garbage;
		static AudioPolicyManager *AudioPolicy;
};


#endif


