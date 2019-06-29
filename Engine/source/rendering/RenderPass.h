#pragma once
#include "GraphicsDeviceInterface.h"
#include "Shader.h"

namespace Baryon
{
class RenderPass : GraphicsDeviceInterface
{
public:
	RenderPass();
	void apply();
	bool setVertexShader(const VertexShader* vertexShader);
	bool setPixelShader(const PixelShader* pixelShader);

	VertexShader* getVertexShader() const;
	PixelShader* getPixelShader() const;
private:
	VertexShader* vertexShader;
	PixelShader* pixelShader;
};

inline RenderPass::RenderPass() : vertexShader{nullptr}, pixelShader{nullptr}
{
}

inline void RenderPass::apply()
{
	vertexShader->apply();
	pixelShader->apply();
}
}
