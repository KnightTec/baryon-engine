#pragma once
#include "System.h"
#include "GraphicsDeviceInterface.h"
#include "Shader.h"

namespace Baryon
{
class VirtualScreen;

class DrawingSystem : public System<StaticMesh>, GraphicsDeviceInterface
{
	using super = System<StaticMesh>;
public:
	explicit DrawingSystem(EntityManager* entityManager, std::vector<VirtualScreen>& virtualScreens);
	void initialize() override;
	void terminate() override;
	void tick() override;
private:
	void update(StaticMesh& staticMesh) override;

	std::vector<VirtualScreen>& virtualScreens;

	VertexShader vs{L"../../Engine/shaders/VertexShader.hlsl", 1};
	PixelShader ps{L"../../Engine/shaders/PixelShader.hlsl"};

	VertexShader postVS{L"../../Engine/shaders/FullscreenVS.hlsl"};
	PixelShader lightPS{L"../../Engine/shaders/LightPS.hlsl", 1};
	PixelShader postPS{L"../../Engine/shaders/PostProcessPS.hlsl", 1};
};
}
