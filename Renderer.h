
#pragma once

#include <d3d12.h>
#include <dxgi1_4.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#include <vector>
#include "GMath.h"
#include "GameUtil.h"
#include "Camera.h"
#include "Shader.h"
#include <memory>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <DirectXTex.h>
#pragma comment(lib, "DirectXTex.lib")

#include <unordered_map>

class Renderer
{
public:
	Renderer(class Game* game, XMFLOAT3 backColor = ZeroVec3d);
	~Renderer();

	bool initialize();

	void begin();
	void end();
	void update(float deltaTime);

	void setBackColor(float r, float g, float b);
	void setBackColor(XMFLOAT3 backColor) { m_backColor = backColor; }
	XMFLOAT3 getBackColor() const { return m_backColor; }

	void setCameraInParam(CamInParam inParam);
	void setCameraExtParam(CamExtParam extParam);
	void setCameraParam(CamInParam inParam, CamExtParam extParam);
	XMMATRIX getCameraMatrix() const { return m_cameraMatrix; }
	XMMATRIX getViewMatrix() const { return m_cameraViewMatrix; }
	XMFLOAT3 getCameraPos() const { return m_camera.getExtParam().eye; }
	XMFLOAT3 getDepthVector();
	Ray getCameraRayOnScreenPos(XMFLOAT2 spos);
	XMFLOAT2 projectToScreenPos(XMFLOAT3 wpos);

	int allocateConstBuffer(const void* pData, size_t dsize, bool meshFlag = true);
	void uploadConstBuffer(int index, const void* pData, size_t dsize);
	void releaseConstBuffer(int index);
	void setCommandCSBufferView(int index);
	ImageData allocateShaderResource(const std::wstring& filePath,
		bool ddsFlag = false);
	void setMaterialSlot(int index, int slotNum, const ImageData& imgData);
	bool drawSprite(int modelIndex, int shaderIndex, ImageData imgData,
		XMFLOAT2 pos, float theta = 0.0f,
		XMFLOAT2* scale = nullptr, XMFLOAT2 offset = ZeroVec2d,
		XMFLOAT2 imgPos = ZeroVec2d, XMFLOAT2* imgScale = nullptr,
		XMFLOAT3 color = Ones3d, float alpha = 1.0f);
	void setShader(int shaderIndex);
	void setPostEffectShader(int shaderIndex);
	ImageData createUnicolorTexture(const wchar_t* imageName, ColorRGBA color);

	bool drawMesh(int shaderIndex, int modelIndex, MeshData meshData,
		int indicesNum, int startPos, int instanceNum = 1);
	bool drawMeshTranslucent(const XMFLOAT3& pos,
		int shaderIndex, int modelIndex, MeshData meshData,
		int indicesNum, int startPos, int instanceNum = 1);

	MeshData getMeshData(const wchar_t* meshName);
	bool loadGmesh(const wchar_t* filePath, std::vector<BufferData>& data);
	bool loadGanim(const wchar_t* filePath, std::vector<BufferData>& data);
	MeshData createGridMesh(const wchar_t* meshName, const GridHeightMap& map);

	void setParallelLight(const ParallelLight& light) { m_parallelLight = light; }
	ParallelLight getParallelLight() const { return m_parallelLight; }

	int allocatePointLight(const PointLight& light);
	void setPointLihgt(int index, const PointLight& light);
	void resetPointLightAll();
	void resetPointLight(int index);

	int allocateSpotLight(const SpotLight& light);
	void setSpotLihgt(int index, const SpotLight& light);
	void resetSpotLightAll();
	void resetSpotLight(int index);

	void setAmbientLight(const XMFLOAT3& light) { m_ambientLight = light; }
	XMFLOAT3 getAmbientLight() const { return m_ambientLight; }

	static const int ShaderNone = -1;
	static const int Shader2DLoopLinear          = 0;
	static const int Shader2DAlphaLoopPoint      = 1;
	static const int Shader2DAddLoopPoint        = 2;
	static const int Shader3DLoopLinear          = 3;
	static const int Shader3DLoopLinearTwoSided  = 4;
	static const int Shader3DClampLinear         = 5;
	static const int Shader3DClampLinearTwoSided = 6;
	static const int Shader3DClampLinearTwoSidedTranslucent = 7;
	static const int Shader3DClampLinearLit      = 8;
	static const int Shader3DLoopLinearLit       = 9;
	static const int Shader3DClampLinearBackTranslucent  = 10;
	static const int Shader3DClampLinearFrontTranslucent = 11;
	static const int Shader3DClampAnimLit        = 12;
	static const int Shader3DClampLinearLitIns   = 13;
	static const int Shader3DClampAnimLitIns     = 14;
	static const int Shader3DClampLinearLitSW    = 15;
	static const int Shader3DClampAnimLitSW      = 16;
	static const int Shader3DNormalMapping       = 17;
	static const int Shader3DNormalMappingLoop   = 18;

	static const int PostEffectNormal = 0;
	static const int PostEffectInvert = 1;

	static const wchar_t* WhiteTexture;
	static const wchar_t* BlackTexture;
	static const wchar_t* GrayTexture;
	static const wchar_t* RedTexture;
	static const wchar_t* GreenTexture;
	static const wchar_t* BlueTexture;
	static const wchar_t* YellowTexture;
	static const wchar_t* CyanTexture;
	static const wchar_t* MagentaTexture;

	static const wchar_t* PlaneMesh;
	static const wchar_t* PlaneTangentMesh;
	static const wchar_t* CubeOuterMesh;
	static const wchar_t* CubeInnerMesh;
	static const wchar_t* HemisphereOuterMesh;
	static const wchar_t* HemisphereInnerMesh;
	static const wchar_t* SphereOuterMesh;
	static const wchar_t* SphereInnerMesh;

private:
	static const UINT FrameNum       = 2;
	static const UINT MaxTextureNum  = 1000;
	static const UINT ConstViewNum   = 2;
	static const UINT TexViewNum     = 6;
	static const UINT ViewNum        = ConstViewNum + TexViewNum;
	static const UINT MaxSpriteNum   = 10000;
	static const UINT MaxMeshNum     = 10000;
	static const UINT MaxObjectNum   = MaxSpriteNum + MaxMeshNum;
	static const UINT SCViewNum      = MaxObjectNum * ViewNum;
	static const UINT ShaderNum      = 19;
	static const UINT PEShaderNum    = 2;
	static const UINT MaxMeshDataNum = 1000;

	static const int ReleaseCountStart = 5;
	std::vector<std::pair<int, int>> m_constBufferReleaseList;

	class Game* m_game;
	ComPtr<IDXGIFactory4> m_dxgiFactory;
	D3D_FEATURE_LEVEL m_featureLevel;
	ComPtr<ID3D12Device>  m_device;
	ComPtr<ID3D12CommandQueue> m_cmdQueue;
	ComPtr<IDXGISwapChain3>    m_swapchain;
	UINT                       m_bufferIndex;
	ComPtr<ID3D12CommandAllocator> m_cmdAllocators[FrameNum];
	ComPtr<ID3D12GraphicsCommandList> m_cmdList;
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	UINT m_rtvIncSize;
	UINT m_csuIncSize;
	ComPtr<ID3D12Resource>       m_backBuffers[FrameNum];
	ComPtr<ID3D12Fence> m_fence;
	UINT64              m_fenceValues[FrameNum];

	XMFLOAT3 m_backColor;
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
	DXGI_FORMAT m_renderTargetFormat;
	ComPtr<ID3D12DescriptorHeap> m_scDescHeap[FrameNum];
	ComPtr<ID3D12Resource> m_textureBuffer[MaxTextureNum];

	ComPtr<ID3D12DescriptorHeap> m_depthHeap;
	ComPtr<ID3D12Resource>       m_depthBuffer;

	UINT m_textureNum;
	std::unordered_map<std::wstring, ImageData> m_textures;
	int m_spriteNum;
	int m_meshNum;
	int m_meshTranslucentNum;

	ComPtr<ID3D12Resource> m_vertexBuffer[MaxMeshDataNum];
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView[MaxMeshDataNum];
	ComPtr<ID3D12Resource> m_indexBuffer[MaxMeshDataNum];
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView[MaxMeshDataNum];
	std::unordered_map<std::wstring, 
		std::vector<BufferData>> m_meshData;

	Camera   m_camera;
	XMMATRIX m_cameraMatrix;
	XMMATRIX m_cameraInverseMatrix;
	XMMATRIX m_cameraViewMatrix;
	XMMATRIX m_spriteMatrix;

	ComPtr<ID3D12Resource>   m_spriteVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_spriteVertexBufferView;

	ComPtr<ID3D12Resource> m_constBuffer[FrameNum][MaxObjectNum];
	void* m_constBufferMap[FrameNum][MaxObjectNum];

	std::unique_ptr<Shader> m_shaders[ShaderNum];
	int                     m_shaderIndex;
	std::unique_ptr<Shader> m_postEffectShaders[PEShaderNum];
	int                     m_postEffectIndex;

	struct SpriteDrawInfo
	{
		int modelIndex;
		int shaderIndex;
		SpriteDrawInfo() : modelIndex(0), shaderIndex(0) {}
		SpriteDrawInfo(int model, int shader) :
			modelIndex(model), shaderIndex(shader) {}
	};
	std::vector<SpriteDrawInfo> m_spriteDrawList;

	struct MeshDrawInfo
	{
		int modelIndex;
		int shaderIndex;
		MeshData meshData;
		int indicesNum;
		int startPos;
		int instanceNum;
		MeshDrawInfo() : modelIndex(0), shaderIndex(0), indicesNum(0),
			startPos(0), instanceNum(0) {}
		MeshDrawInfo(int model, int shader, const MeshData& data, 
			int inum, int spos, int instNum) : modelIndex(model), 
			shaderIndex(shader), meshData(data), indicesNum(inum), 
			startPos(spos), instanceNum(instNum) {}
	};
	std::vector<MeshDrawInfo> m_meshDrawList;
	std::vector<MeshDrawInfo> m_meshDrawTranslucentList;
	std::vector<std::pair<float, int>> m_meshTranslucentIndex;

	ComPtr<ID3D12Resource> m_sceneConstBuffer[FrameNum];
	void*                  m_sceneConstBufferMap[FrameNum];

	ComPtr<ID3D12Resource>       m_offscreenBuffer;
	ComPtr<ID3D12DescriptorHeap> m_offscreenRTVHeap;
	ComPtr<ID3D12DescriptorHeap> m_offscreenSRVHeap;
	ComPtr<ID3D12Resource>       m_offscreenVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW     m_offscreenVertexBufferView;

	ParallelLight m_parallelLight;
	PointLight m_pointLight[MaxPointLightNum];
	bool m_availablePointLight[MaxPointLightNum];
	int m_pointLightNum;
	SpotLight m_spotLight[MaxSpotLightNum];
	bool m_availableSpotLight[MaxSpotLightNum];
	int m_spotLightNum;
	XMFLOAT3 m_ambientLight;

	bool createFactory();
	bool createDevice(const wchar_t* adapterName);
	bool createCommandQueue();
	bool createSwapchain();
	bool createCommandAllocators();
	bool createCommandList();
	bool createRenderTargetView();
	bool createFence();
	void moveToNextFrame();
	void waitForGPU();
	void setResourceBarrier(ID3D12Resource* buffer,
		D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

	void enableDebugLayer();
	void setViewport(D3D12_VIEWPORT& viewport);
	void setScissorRect(D3D12_RECT& scissor);

	bool createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize);
	bool uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,
		void** map = nullptr);
	void setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
		ID3D12Resource* buffer, UINT bSize, UINT stride);
	void setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
		ID3D12Resource* buffer, UINT bSize);
	bool createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
		DXGI_FORMAT format, UINT16 mipLevels = 1, UINT16 depthOrArraySize = 1,
		D3D12_RESOURCE_DIMENSION dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		D3D12_CLEAR_VALUE* clearValue = nullptr);
	bool uploadShaderResource(ID3D12Resource* buffer, void* src,
		UINT lineSize, UINT allSize);
	bool createDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum);
	void setShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
		ID3D12DescriptorHeap* dHeap, UINT index);
	bool readImageFile(ID3D12Resource** buffer, TexMetadata& metadata,
		const wchar_t* filePath, bool ddsFlag = false);
	bool createConstBuffer(ID3D12Resource** buffer, const void* src,
		size_t dsize, void** pmap = nullptr);
	void setConstBufferView(ID3D12Resource* buffer,
		ID3D12DescriptorHeap* dHeap, UINT index);
	UINT64 calcAlignment256(size_t size);

	void drawBatchSprite();
	void drawBatchMesh();
	void drawBatchMeshTranslucent();

	bool createRTVDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum);
	void setRenderTargetView(ID3D12Resource* buffer, DXGI_FORMAT format,
		ID3D12DescriptorHeap* dHeap, UINT index);
	bool createOffscreen();

	bool createDepthBuffer(ID3D12Resource** buffer);
	bool createDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth);

	bool createPlaneMesh(const wchar_t* meshName, float width = 1.0f,
		int xNum = 2, int yNum = 2, bool tangentFlag = false);
	bool createCubeMesh(const wchar_t* meshName, bool outer, float width = 1.0f);
	bool createHemisphereMesh(const wchar_t* meshName, bool outer, float radius = 0.5f,
		UINT nv = 20, UINT nh = 30, float cu = 0.5f, float cv = 0.5f, float uvR = 0.5f);
	bool createSphereMesh(const wchar_t* meshName, bool outer, float radius = 0.5f,
		UINT nv = 20, UINT nh = 30);

};

