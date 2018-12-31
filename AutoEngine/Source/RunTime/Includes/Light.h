#pragma once
#include "Math/Vector3.h"
#include "Math/Color.h"
#include "Transform.h"
#include "Object.h"
namespace Auto3D {

enum class LightType
{
	Directional,
	Point,
	Spot
};
enum class ShadowType
{
	NoShadow,
	Soft,
	Hard
};
/**
* Manage this light create shadow
*/
class ShadowRenderAssist : public Object
{
	REGISTER_OBJECT_CLASS(ShadowRenderAssist, Object)
public:
	/**
	* @brief : Set light type, default Directional
	*/
	explicit ShadowRenderAssist(SharedPtr<Ambient> ambient, LightType type = LightType::Directional);
	/**
	* @brief : Bind light create depath map
	*/
	void BindDepathMap();
	/**
	* @brief : Get shadow width
	*/
	int GetShadowWidth() { return _shadowWidth; }
	/**
	* @brief : Get shadow height
	*/
	int GetShadowHeight() { return _shadowHeight; }
	/**
	* @brief : Get depth map fbo
	*/
	unsigned GetDepthMapFBO();
	/**
	* @brief : Get depth map
	*/
	unsigned GetDepthMap();

private:
	LightType _type;
	int _shadowWidth;
	int _shadowHeight;

	unsigned _depthDirMapFBO;
	unsigned _depthDirMap;

	unsigned _depthPointMapFBO;
	unsigned _depthPointmap;
};
/**
* @brief : Light component
*/
class Light : public Component, public EnableSharedFromThis<Light>
{
	REGISTER_OBJECT_CLASS(Light, Component)
public:
	explicit Light(SharedPtr<Ambient> ambi);
	/**
	* @brief : Register object factory.
	*/
	static void RegisterObject(SharedPtr<Ambient> ambient);
	void Init()override;
	/**
	* @brief : Update light data
	*/
	void Update()override;
	/**
	* @brief : Set light type
	*/
	void SetType(LightType type);
	/**
	* @brief : Get light type
	*/
	LightType GetType() const { return _type; }
	/**
	* @brief : Set shadow type and create shadow assist
	*/
	void SetShadowType(ShadowType type);
	/**
	* @brief : Get light space matrix (projection * view)
	*/
	glm::mat4& GetLightSpaceMatrix();
	/**
	* @brief : Get light projection matrix (ortho)
	*/
	glm::mat4& GetLightProjectionMatrix();
	/**
	* @brief : Get light space matrix (lookAt Vector::ZERO)
	*/
	glm::mat4& GetLightViewMatrix();
	/**
	* @brief : Get light component position
	*/
	glm::vec3 GetLightPosition();
	/**
	* @brief : Get Shadow render assist
	*/
	ShadowRenderAssist* GetShadowAssist() { return _shadowAssist.get(); }
	/**
	* @brief : Get light far plane
	*/
	float GetFarPlane() { return _farPlane; }
	/**
	* @brief : Get light near plane
	*/
	float GetNearPlane() { return _nearPlane; }
	/**
	* @brief : Get light direction
	*/
	Vector3 GetDirection() { return  Vector3(-direction.x, direction.y, direction.z); }
	/**
	* @brief :Get light color 
	*/
	Color& GetColor() { return _color; }
	/**
	* @brief :Get light color to vector3 (r,g,b)->(x,y,z)
	*/
	Vector3 GetColorToVec() { return Vector3(_color.r, _color.g, _color.b); }
	/**
	* @brief : Set color with float (x,y,z)->(r,g,b)
	*/
	void SetColor(float x, float y, float z) { _color.Set(x, y, z); }
	/**
	* @brief : Set color with Vector3 (x,y,z)->(r,g,b)
	*/
	void SetColor(const Vector3& vec) { _color.Set(vec.x, vec.y, vec.z); }
	/**
	* @brief : Set color
 	*/
	void SetColor(const Color& color) { _color = color; }


	void AddToManager();
	void RemoveFromManager();


	Color _color;
	Vector3 direction;

	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;

	Vector3 ambient;
	Vector3 diffuse;
	Vector3 specular;

protected:	
	SharedPtr<ShadowRenderAssist> _shadowAssist;
	ShadowType _shadowType;
	glm::mat4 _lightSpaceMatrix;
	glm::mat4 _lightProjectionMatrix;
	glm::mat4 _lightViewMatrix;
	///shadow far plane
	float _farPlane;
	float _nearPlane;
	///light type
	LightType _type;

};

}

