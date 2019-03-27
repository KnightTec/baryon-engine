#include "Renderer.h"
#include "VirtualScreen.h"

using namespace Baryon;
using namespace Microsoft::WRL;

void Renderer::render()
{
	for (VirtualScreen* screen : virtualScreens)
	{
		if (screen) {
			ID3D11RenderTargetView* rtv = screen->getRenderTargetView();
			getContext().OMSetRenderTargets(1, &rtv, screen->getDepthStencilView());

			const float clearColor[] = { 0.739f, 0.339f, 0.139f, 1.000f };
			getContext().ClearRenderTargetView(screen->getRenderTargetView(), clearColor);

			screen->present();
		}
	}
}
