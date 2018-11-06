#include "StateVessel.h"
#include "Ambient.h"

namespace Auto3D {

StateVessel::StateVessel(Ambient* ambient)
	:Super(ambient)
{
}


StateVessel::~StateVessel()
{
}


#if SharedPtrDebug
SharedPtr<Object> StateVessel::CreateObject(__String type)
{
	return _ambient->CreateObject(type);
}
#else
Object* StateVessel::CreateObject(__String type)
{
	return _ambient->CreateObject(type);
}
#endif



}