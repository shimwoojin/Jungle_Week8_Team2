#include "ShadowDepth.h"
#include "RenderPassRegistry.h"

#include "Render/Device/D3DDevice.h"

REGISTER_RENDER_PASS(FShadowDepthPass)
#include "Render/Types/FrameContext.h"
#include "Render/Types/RenderConstants.h"
#include "Render/Command/DrawCommandList.h"
#include "Render/Resource/RenderResources.h"

FShadowDepthPass::FShadowDepthPass() {
	PassType    = ERenderPass::ShadowDepth;
	RenderState = { EDepthStencilState::Default, EBlendState::NoColor,
					ERasterizerState::SolidFrontCull, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true };
}

void FShadowDepthPass::BeginPass(const FPassContext& Ctx) {
	// 1. Reset quadtree for this frame
	Quadtree.Reset();
	AtlasEntries.clear();

	// 2. Allocate atlas regions for each visible spot light
	ID3D11DeviceContext* DC = Ctx.Device.GetDeviceContext();
	const FFrameContext& Frame = Ctx.Frame;
	FStateCache& Cache = Ctx.Cache;

	const auto& Lights = Ctx.Resources.CachedLightInfos;
	for (uint32 i = 0; i < Lights.size(); i++) {
		const FLightInfo& Light = Lights[i];
		if (Light.LightType != ELightType::Spot) continue;

		FAtlasRegion Region = Quadtree.Add(Light);
		if (!Region.bValid) continue;

		FLightAtlasEntry Entry;
		Entry.LightViewProjection = ComputeLightViewProj(Light);
		Entry.UOffset = Region.X / Quadtree.GetAtlasSize();
		Entry.VOffset = Region.Y / Quadtree.GetAtlasSize();
		Entry.Resolution = Region.Size;
		Entry.LightId = i;
		AtlasEntries.push_back(Entry);
	}

	// 3. Clear and bind shadow atlas DSV
	DC->ClearDepthStencilView(Ctx.Frame.ShadowAtlasDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	DC->OMSetRenderTargets(0, nullptr, Ctx.Frame.ShadowAtlasDSV);
	Ctx.Cache.bForceAll = true;
}

void FShadowDepthPass::EndPass(const FPassContext& Ctx) {

}