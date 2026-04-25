#include "ShadowDepth.h"
#include "RenderPassRegistry.h"

#include "Render/Device/D3DDevice.h"

REGISTER_RENDER_PASS(FShadowDepthPass)
#include "Render/Types/FrameContext.h"
#include "Render/Types/RenderConstants.h"
#include "Render/Command/DrawCommandList.h"

FShadowDepthPass::FShadowDepthPass() {
	PassType    = ERenderPass::ShadowDepth;
	RenderState = { EDepthStencilState::Default, EBlendState::NoColor,
					ERasterizerState::SolidFrontCull, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, true };
}

void FShadowDepthPass::BeginPass(const FPassContext& Ctx) {

}

void FShadowDepthPass::EndPass(const FPassContext& Ctx) {

}