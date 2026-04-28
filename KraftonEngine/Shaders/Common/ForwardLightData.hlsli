#ifndef FORWARD_LIGHT_DATA_HLSLI
#define FORWARD_LIGHT_DATA_HLSLI

// =============================================================================
// Forward Shading 라이팅 구조체 & 리소스 바인딩
// C++ ForwardLightData.h 와 바이트 단위로 1:1 대응
//
// 슬롯 배치:
//   b4        LightingBuffer (Ambient + Directional + 메타)
//   t8        StructuredBuffer<FLightInfo>  (Point/Spot 통합)
//   t9        StructuredBuffer<uint>        (TileLightIndices)
//   t10       StructuredBuffer<uint2>       (TileLightGrid)
// =============================================================================

// ── Light Type 상수 ──
#define LIGHT_TYPE_POINT 0
#define LIGHT_TYPE_SPOT  1

// ── Tile Culling 상수 ──
#define TILE_SIZE             16
#define MAX_LIGHTS_PER_TILE   256

#define LIGHT_CULLING_OFF     0
#define LIGHT_CULLING_TILE    1
#define LIGHT_CULLING_CLUSTER 2

// =============================================================================
// 구조체 — C++ POD와 레이아웃 동일
// =============================================================================
struct FAABB
{
    float4 minPt;
    float4 maxPt;
};
struct FAmbientLightInfo
{
    float4 Color; // 16B
    float Intensity; //  4B
    float3 _padA; // 12B → 32B
};

struct FDirectionalLightInfo
{
    float4 Color; // 16B
    float3 Direction; // 12B
    float Intensity; //  4B → 32B
};

// Point/Spot 통합 POD — LightType으로 분기
struct FLightInfo
{
    float4 Color; // 16B

    float3 Position; // 12B
    float Intensity; //  4B

    float AttenuationRadius; //  4B
    float FalloffExponent; //  4B
    uint LightType; //  4B
    float _pad0; //  4B

    float3 Direction; // 12B  (Spot 전용)
    float InnerConeCos; //  4B  (Spot 전용)

    float OuterConeCos; //  4B  (Spot 전용)
    uint ShadowMapIndex; //  4B  (Atlas/array index)
    uint bCastShadow; //  4B  (0 or 1)
    float _pad1; //  4B

    float4 ShadowAtlasScaleBias; // 16B  (Atlas UV transform) → 합계 96B
};
struct FClusterCullingState
{
    float NearZ;
    float FarZ;
    uint ClusterX;
    uint ClusterY;

    uint ClusterZ;
    uint ScreenWidth;
    uint ScreenHeight;
    uint MaxLightsPerCluster;

    uint bIsOrtho;
    float OrthoWidth;
    float2 _pad;
};

// =============================================================================
// 리소스 바인딩
// =============================================================================

// ── Lighting CB (b4) — Ambient + Directional + 메타데이터 ──
cbuffer LightingBuffer : register(b4)
{
    FAmbientLightInfo AmbientLight;
    FDirectionalLightInfo DirectionalLight;

    uint NumActivePointLights;
    uint NumActiveSpotLights;
    uint NumTilesX;
    uint NumTilesY;
    FClusterCullingState CullState;
    uint LightCullingMode;
    uint VisualizeLightCulling;
    float HeatMapMax;
    uint Pad;
};

// ── Structured Buffers (t8~t10) ──
StructuredBuffer<FLightInfo> AllLights : register(t8);
StructuredBuffer<uint> TileLightIndices : register(t9);
StructuredBuffer<uint2> TileLightGrid : register(t10);
StructuredBuffer<uint> g_ClusterLightIndices : register(t11);
StructuredBuffer<uint2> g_ClusterLightGrid : register(t12);

// =============================================================================
// Per-light Shadow 구조체 — C++ FSpotShadowDataGPU / FPointShadowDataGPU 와 1:1 대응
// StructuredBuffer로 바인딩 (CB 64KB 제한 회피)
// =============================================================================

// Spot Light: ViewProj + atlas UV rect + page(slice) index  (96B)
struct FSpotShadowData
{
    float4x4 ViewProj;          // 64B
    float4   AtlasScaleBias;    // 16B  (xy=scale, zw=bias)
    uint     PageIndex;         //  4B  (Texture2DArray slice)
    float    ShadowBias;        //  4B
    float    ShadowSharpen;     //  4B
    float    ShadowSlopeBias;   //  4B  → 합계 96B
};

// Point Light: 6면 ViewProj + per-face atlas UV rect  (512B, 32B aligned)
struct FPointShadowData
{
    float4x4 FaceViewProj[6];          // 384B
    float4   FaceAtlasScaleBias[6];    //  96B  (xy=scale, zw=bias, one per face)
    float    NearZ;                    //   4B
    float    FarZ;                     //   4B
    float    ShadowBias;               //   4B
    float    ShadowSharpen;            //   4B
    float    ShadowSlopeBias;          //   4B
    float    ShadowNormalBias;         //   4B
    float    _pad[2];                  //  12B  → 합계 512B
};

// ── Per-light Shadow StructuredBuffers (t24, t25) ──
StructuredBuffer<FSpotShadowData>  SpotShadowDatas  : register(t24);
StructuredBuffer<FPointShadowData> PointShadowDatas : register(t25);

#endif // FORWARD_LIGHT_DATA_HLSLI
