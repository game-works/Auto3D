#pragma once
#include "AudioNode.h"

namespace Auto3D
{

class Audio;

class AUTO_API AudioListener : public AudioNode
{
	REGISTER_OBJECT_CLASS(AudioListener, AudioNode)

public:
	AudioListener();
	~AudioListener();


	/// Register factory and attributes.
	static void RegisterObject();

	const Vector3F GetVel()const { return _listenerVel; }
	const Vector3F GetOriAt()const { return _listenerOriAt; }
	const Vector3F GetOriUp()const { return _listenerOriUp; }
private:
	//Listener speed
	Vector3F _listenerVel;
	// Listener direction elements are at
	Vector3F _listenerOriAt;
	// Listener direction elements are up
	Vector3F _listenerOriUp;
};

}