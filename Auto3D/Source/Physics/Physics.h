#pragma once
#include "../Object/GameManager.h"

namespace Auto3D {

/// Physics sub system 
class AUTO_API Physics : public BaseSubsystem
{
	REGISTER_OBJECT_CLASS(Physics, BaseSubsystem)
public:
	/// Construct
	Physics();

	~Physics();

};


}