#pragma once
#include "Component.h"
#include "AutoOAL.h"
#include "AudioClip.h"

namespace Auto3D {

enum class AudioSourceState
{
	DEFAULT,
	INITIAL,
	PLAYING,
	PAUSED,
	STOPPED
};

class AudioSource : public Component
{
	REGISTER_DERIVED_CLASS(AudioSource, Component);
	DECLARE_OBJECT_SERIALIZE(AudioSource);
public:
	explicit AudioSource(Ambient* ambient);
	/**
	* @brief : Plays the active audioclip at (future) scheduled time.
				If time < 0 it specifies a delay
	*/
	void Play(double delayTime = 0.0);
	/**
	* @brief : Pauses the active audioclip
	*/
	void Pause();
	/**
	* @brief : Stops the active audio clip
	*/
	void Stop(bool enable);

	/**
	* @brief : Set audio loop
	*/
	void SetLoop(bool enable);
	/**
	* @brief : Is the audio source currently playing
	*/
	bool IsPlaying() const { return _isPlaying; }
	/**
	* @brief : Is the audio source currently paused
	*/
	bool IsPaused() const { return _isPaused; }
	/**
	* @brief : Is the audio source currently stop
	*/
	bool IsStop() const { return _isStop; }
	/**
	* @brief : Get audio source state with AudioSourceState
	*/
	AudioSourceState GetState();
private:
	void attachBuffer(const AudioClip& clip);

	void attachBuffer(AudioClip* clip);
private:
	/// is playing in this audio source
	bool _isPlaying{};
	///	is pause in this audio source
	bool _isPaused{};
	/// is stop int this audio source
	bool _isStop{};

	bool _isLoop{};

	ALuint _uiBuffer;

	ALuint _uiSource;

	ALint _iState;
};

}