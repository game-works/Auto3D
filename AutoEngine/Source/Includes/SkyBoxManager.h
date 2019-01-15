#pragma once


namespace Auto3D {
class SkyBox;
class SkyManager
{
public:
	SkyManager() = default;
	~SkyManager() = default;
	void AddSkyBox(SkyBox* skybox) { _skybox = skybox; }
	SkyBox* GetSkyBox() { return _skybox; }
private:
	SkyBox* _skybox;
};

}