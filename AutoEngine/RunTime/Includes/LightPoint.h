#pragma once
#include "Light.h"
AUTO_BEGIN
class LightPoint : public Light
{
	REGISTER_DERIVED_CLASS(LightPoint, Light);
	DECLARE_OBJECT_SERIALIZE(LightPoint);
public:
	LightPoint();
private:

	/*Color color;
	float constant;
	float linear;
	float quadratic;
	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;*/

};

AUTO_END