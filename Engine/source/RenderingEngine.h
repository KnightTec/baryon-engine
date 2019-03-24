#pragma once
#include "Viewport.h"

#include "wrl/client.h"
#include "d3d11_4.h"
#include <vector>


namespace Baryon
{
class Engine;

class RenderingEngine
{
	friend class Engine;
public:
	bool createViewport(const Window& window, Viewport* outViewport);
private:
	RenderingEngine() = default;
	bool startUp();
	void render();

	static bool initialized;
	std::vector<Viewport> viewports;
	Microsoft::WRL::ComPtr<ID3D11Device4> d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext4> d3dContext;
};
}
