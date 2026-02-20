
#include "Renderer.h"
#include "Game.h"
#include <fstream>
#include <type_traits>

const wchar_t* Renderer::WhiteTexture(L"White");
const wchar_t* Renderer::BlackTexture(L"Black");
const wchar_t* Renderer::GrayTexture(L"Gray");
const wchar_t* Renderer::RedTexture(L"Red");
const wchar_t* Renderer::GreenTexture(L"Green");
const wchar_t* Renderer::BlueTexture(L"Blue");
const wchar_t* Renderer::YellowTexture(L"Yellow");
const wchar_t* Renderer::CyanTexture(L"Cyan");
const wchar_t* Renderer::MagentaTexture(L"Magenta");

const wchar_t* Renderer::PlaneMesh(L"Plane");
const wchar_t* Renderer::PlaneTangentMesh(L"PlaneTangent");
const wchar_t* Renderer::CubeOuterMesh(L"Cube_Outer");
const wchar_t* Renderer::CubeInnerMesh(L"Cube_Inner");
const wchar_t* Renderer::HemisphereOuterMesh(L"Hemisphere_Outer");
const wchar_t* Renderer::HemisphereInnerMesh(L"Hemisphere_Inner");
const wchar_t* Renderer::SphereOuterMesh(L"Sphere_Outer");
const wchar_t* Renderer::SphereInnerMesh(L"Sphere_Inner");

Renderer::Renderer(Game* game, XMFLOAT3 backColor)
	: m_game(game)
	, m_featureLevel()
	, m_bufferIndex(0)
	, m_rtvIncSize(0)
	, m_csuIncSize(0)
	, m_renderTargetFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
	, m_backColor(backColor)
	, m_fenceValues{ 0 }
	, m_camera(CamInParam(90.0f, 0.01f, 1000.0f),
		CamExtParam(ZeroVec3d, UnitVecZ3d, UnitVecY3d))
	, m_cameraMatrix(XMMatrixIdentity())
	, m_cameraInverseMatrix(XMMatrixIdentity())
	, m_cameraViewMatrix(XMMatrixIdentity())
	, m_spriteMatrix(XMMatrixIdentity())
	, m_spriteVertexBufferView{}
	, m_constBufferMap{}
	, m_textureNum(0)
	, m_spriteNum(0)
	, m_meshNum(0)
	, m_meshTranslucentNum(0)
	, m_shaderIndex(ShaderNone)
	, m_postEffectIndex(PostEffectNormal)
	, m_spriteDrawList(MaxSpriteNum)
	, m_meshDrawList(MaxMeshNum)
	, m_meshDrawTranslucentList(MaxMeshNum)
	, m_meshTranslucentIndex(MaxMeshNum)
	, m_vertexBufferView{}
	, m_indexBufferView{}
	, m_sceneConstBufferMap{}
	, m_ambientLight(Ones3d)
	, m_offscreenVertexBufferView{}
{
	resetPointLightAll();
	resetSpotLightAll();
}

Renderer::~Renderer()
{
	waitForGPU();

	CoUninitialize();
}

bool Renderer::initialize()
{
#ifdef _DEBUG
	enableDebugLayer();
#endif

	{
		HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
		if (FAILED(hr)) return false;
	}

	if (!createFactory()) return false;
	if (!createDevice(L"NVIDIA"))   return false;
	if (!createCommandQueue())      return false;
	if (!createSwapchain())         return false;
	if (!createCommandAllocators()) return false;
	if (!createCommandList())       return false;
	if (!createRenderTargetView())  return false;
	if (!createFence())             return false;
	setViewport(m_viewport);
	setScissorRect(m_scissorRect);

	m_cmdList->Close();

	// 入力レイアウト、ルートシグネチャ、PSO
	D3D12_INPUT_ELEMENT_DESC inputLayouts[5];
	inputLayouts[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[3] = { "BONENO", 0, DXGI_FORMAT_R16G16B16A16_UINT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayouts[4] = { "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_ELEMENT_DESC inputLayoutsTangent[4];
	inputLayoutsTangent[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayoutsTangent[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayoutsTangent[2] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputLayoutsTangent[3] = { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	m_shaders[Shader2DLoopLinear] = std::make_unique<Shader>(
		m_device.Get(),	L"shader\\simpleVS.cso", L"shader\\simplePS.cso",
		inputLayouts, 2, m_renderTargetFormat,
		true, true, Shader::BlendConfig::None, false);
	m_shaders[Shader2DAlphaLoopPoint] = std::make_unique<Shader>(
		m_device.Get(),	L"shader\\simpleVS.cso", L"shader\\simplePS.cso",
		inputLayouts, 2, m_renderTargetFormat,
		true, false, Shader::BlendConfig::Alpha, false);
	m_shaders[Shader2DAddLoopPoint] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\simpleVS.cso", L"shader\\simplePS.cso",
		inputLayouts, 2, m_renderTargetFormat,
		true, false, Shader::BlendConfig::Add, false);
	m_shaders[Shader3DLoopLinear] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		true, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DLoopLinearTwoSided] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		true, true, Shader::BlendConfig::None, true, Shader::CullConfig::None);
	m_shaders[Shader3DClampLinear] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampLinearTwoSided] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::None);
	m_shaders[Shader3DClampLinearTwoSidedTranslucent] =
		std::make_unique<Shader>(m_device.Get(), 
			L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::Alpha, true, Shader::CullConfig::None);
	m_shaders[Shader3DClampLinearLit] =	std::make_unique<Shader>(
		m_device.Get(),	L"shader\\LitMeshVS.cso", L"shader\\LitMeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DLoopLinearLit] = std::make_unique<Shader>(
		m_device.Get(), L"shader\\LitMeshVS.cso", L"shader\\LitMeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		true, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampLinearBackTranslucent] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::Alpha, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampLinearFrontTranslucent] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\MeshVS.cso", L"shader\\MeshPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::Alpha, true, Shader::CullConfig::Front);
	m_shaders[Shader3DClampAnimLit] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\LitAnimMeshVS.cso", L"shader\\LitAnimMeshPS.cso",
		inputLayouts, 5, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampLinearLitIns] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\LitMeshInsVS.cso", L"shader\\LitMeshPS.cso",
		inputLayouts, 5, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampAnimLitIns] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\LitAnimMeshInsVS.cso", L"shader\\LitAnimMeshPS.cso",
		inputLayouts, 5, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampLinearLitSW] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\LitMeshShadowVS.cso", L"shader\\LitMeshShadowPS.cso",
		inputLayouts, 3, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DClampAnimLitSW] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\LitAnimMeshShadowVS.cso", L"shader\\LitAnimMeshShadowPS.cso",
		inputLayouts, 5, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DNormalMapping] =
		std::make_unique<Shader>(m_device.Get(),
		L"shader\\NormalMapVS.cso", L"shader\\NormalMapPS.cso",
		inputLayoutsTangent, 4, m_renderTargetFormat,
		false, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	m_shaders[Shader3DNormalMappingLoop] =
		std::make_unique<Shader>(m_device.Get(),
			L"shader\\NormalMapVS.cso", L"shader\\NormalMapPS.cso",
			inputLayoutsTangent, 4, m_renderTargetFormat,
			true, true, Shader::BlendConfig::None, true, Shader::CullConfig::Back);
	for (int i = 0; i < ShaderNum; ++i)
	{
		if (!m_shaders[i]->isEnabled()) return false;
	}

	m_postEffectShaders[PostEffectNormal] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\PostEffectVS.cso", L"shader\\PostEffectPS.cso", inputLayouts, 2,
		m_renderTargetFormat, false, false, Shader::BlendConfig::None,
		false, Shader::CullConfig::None);
	m_postEffectShaders[PostEffectInvert] = std::make_unique<Shader>(m_device.Get(),
		L"shader\\PostEffectVS.cso", L"shader\\PostEffectInvertPS.cso", inputLayouts, 2,
		m_renderTargetFormat, false, false, Shader::BlendConfig::None,
		false, Shader::CullConfig::None);
	for (int i = 0; i < PEShaderNum; ++i)
	{
		if (!m_postEffectShaders[i]->isEnabled()) return false;
	}

	if (!createDepthBuffer(m_depthBuffer.GetAddressOf())) return false;
	if (!createDepthHeap(m_depthHeap.GetAddressOf(), m_depthBuffer.Get())) return false;
	
	// 頂点バッファの生成
	VertexUV vertices[4];
	vertices[0] = { { -0.5f, -0.5f, 0.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f } };
	vertices[3] = { {  0.5f,  0.5f, 0.0f }, { 1.0f, 0.0f } };
	if (!createResourceBuffer(m_spriteVertexBuffer.GetAddressOf(),
		4 * sizeof(VertexUV))) return false;
	if (!uploadResourceBuffer(m_spriteVertexBuffer.Get(),
		(void*)vertices, 4 * sizeof(VertexUV))) return false;
	setVertexBufferView(m_spriteVertexBufferView, m_spriteVertexBuffer.Get(),
		4 * sizeof(VertexUV), sizeof(VertexUV));

	// ディスクリプタヒープの生成
	for (int i = 0; i < FrameNum; ++i)
	{
		if (!createDescHeap(m_scDescHeap[i].GetAddressOf(), SCViewNum)) return false;
	}

	// シーン共通定数バッファ
	{
		SceneData sd;
		for (int i = 0; i < FrameNum; ++i)
		{
			if (!createConstBuffer(m_sceneConstBuffer[i].GetAddressOf(),
				(void*)&sd, sizeof(SceneData),
				&m_sceneConstBufferMap[i])) return false;
			for (int j = 0; j < MaxObjectNum; ++j)
			{
				setConstBufferView(m_sceneConstBuffer[i].Get(),
					m_scDescHeap[i].Get(), (UINT)(j * ViewNum + 1));
			}
		}
	}

	// カメラ行列を計算
	m_cameraMatrix = m_camera.calcProjMatrix(
		(float)m_game->getWidth(), (float)m_game->getHeight());
	m_cameraViewMatrix = m_camera.calcViewMatrix();
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);

	// スプライト行列を計算
	m_spriteMatrix = 
		calcSpriteMatrix((float)m_game->getWidth(), (float)m_game->getHeight());

	// オフスクリーン生成
	if (!createOffscreen()) return false;

	// 単色テクスチャの生成
	ImageData imgData;
	imgData = createUnicolorTexture(WhiteTexture, ColorRGBA(255, 255, 255, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(BlackTexture, ColorRGBA(0, 0, 0, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(GrayTexture, ColorRGBA(127, 127, 127, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(RedTexture,	ColorRGBA(255, 0, 0, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(GreenTexture, ColorRGBA(0, 255, 0, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(BlueTexture, ColorRGBA(0, 0, 255, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(YellowTexture, ColorRGBA(255, 255, 0, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(CyanTexture, ColorRGBA(0, 255, 255, 255));
	if (imgData.imgIndex == -1) return false;
	imgData = createUnicolorTexture(MagentaTexture, ColorRGBA(255, 0, 255, 255));
	if (imgData.imgIndex == -1) return false;

	// プリミティブメッシュの生成
	if (!createPlaneMesh(PlaneMesh, 1.0f, 101, 101)) return false;
	if (!createPlaneMesh(PlaneTangentMesh, 1.0f, 101, 101, true)) return false;
	if (!createCubeMesh(CubeOuterMesh, true, 1.0f)) return false;
	if (!createCubeMesh(CubeInnerMesh, false, 1.0f)) return false;
	if (!createHemisphereMesh(HemisphereOuterMesh, true, 0.5f, 40, 60,
		0.5f, 0.5f, 0.5f)) return false;
	if (!createHemisphereMesh(HemisphereInnerMesh, false, 0.5f, 40, 60,
		0.5f, 0.5f, 0.5f)) return false;
	if (!createSphereMesh(SphereOuterMesh, true, 0.5f, 40, 60)) return false;
	if (!createSphereMesh(SphereInnerMesh, false, 0.5f, 40, 60)) return false;

	return true;
}

void Renderer::begin()
{
	m_cmdAllocators[m_bufferIndex]->Reset();
	m_cmdList->Reset(m_cmdAllocators[m_bufferIndex].Get(), nullptr);

	//setResourceBarrier(D3D12_RESOURCE_STATE_PRESENT,
	//	D3D12_RESOURCE_STATE_RENDER_TARGET);
	setResourceBarrier(m_offscreenBuffer.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	//auto rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	//rtvHandle.ptr += m_bufferIndex * m_rtvIncSize;
	auto rtvHandle = m_offscreenRTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvHandle = m_depthHeap->GetCPUDescriptorHandleForHeapStart();
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

	float clearColor[4] = { m_backColor.x, m_backColor.y, m_backColor.z, 1.0f };
	m_cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	m_cmdList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH,
		1.0f, 0, 0, nullptr);

	m_cmdList->RSSetViewports(1, &m_viewport);
	m_cmdList->RSSetScissorRects(1, &m_scissorRect);

	m_shaderIndex = ShaderNone;
	m_cmdList->SetDescriptorHeaps(1, m_scDescHeap[m_bufferIndex].GetAddressOf());

	{
		SceneData sd;
		sd.viewProj = m_cameraMatrix;
		sd.eye = m_camera.getExtParam().eye;
		sd.parallelLight = m_parallelLight;
		memcpy((void*)sd.pointLight, m_pointLight, MaxPointLightNum * sizeof(PointLight));
		memcpy((void*)sd.spotLight, m_spotLight, MaxSpotLightNum * sizeof(SpotLight));
		sd.ambientLight = m_ambientLight;
		memcpy(m_sceneConstBufferMap[m_bufferIndex], (void*)&sd, sizeof(SceneData));
	}
}

void Renderer::end()
{
	// メッシュ描画（不透過）
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	drawBatchMesh();
	// メッシュ描画（半透過）
	drawBatchMeshTranslucent();
	// スプライト描画
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_cmdList->IASetVertexBuffers(0, 1, &m_spriteVertexBufferView);
	drawBatchSprite();

	m_spriteNum = 0;
	m_meshNum = 0;
	m_meshTranslucentNum = 0;

	//setResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET,
	//	D3D12_RESOURCE_STATE_PRESENT);
	setResourceBarrier(m_offscreenBuffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	setResourceBarrier(m_backBuffers[m_bufferIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, 
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// ポストプロセス
	m_postEffectShaders[m_postEffectIndex]->setShader(m_cmdList.Get());
	m_cmdList->SetDescriptorHeaps(1, m_offscreenSRVHeap.GetAddressOf());
	auto handle = m_offscreenSRVHeap->GetGPUDescriptorHandleForHeapStart();
	m_cmdList->SetGraphicsRootDescriptorTable(0, handle);
	auto rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += m_bufferIndex * m_rtvIncSize;
	m_cmdList->OMSetRenderTargets(1, &rtvHandle, true, nullptr);
	m_cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_cmdList->IASetVertexBuffers(0, 1, &m_offscreenVertexBufferView);
	m_cmdList->DrawInstanced(4, 1, 0, 0);

	setResourceBarrier(m_backBuffers[m_bufferIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, 
		D3D12_RESOURCE_STATE_PRESENT);

	m_cmdList->Close();
	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(1, cmdLists);
	m_swapchain->Present(1, 0);
	moveToNextFrame();
}

void Renderer::update(float deltaTime)
{
	{
		auto it = m_constBufferReleaseList.begin();
		while (it != m_constBufferReleaseList.end())
		{
			(*it).second -= 1;
			if ((*it).second <= 0)
			{
				int index = (*it).first;
				for (int i = 0; i < FrameNum; ++i)
				{
					m_constBuffer[i][index]->Unmap(0, nullptr);
					m_constBuffer[i][index].Reset();
				}
				it = m_constBufferReleaseList.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

bool Renderer::createFactory()
{
	UINT debugFlag = 0;
#ifdef _DEBUG
	debugFlag = DXGI_CREATE_FACTORY_DEBUG;
#endif

	HRESULT hr = CreateDXGIFactory2(debugFlag, IID_PPV_ARGS(m_dxgiFactory.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createDevice(const wchar_t* adapterName)
{
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* sa = nullptr;
	int i = 0;
	while (m_dxgiFactory->EnumAdapters(i, &sa) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(sa);
		++i;
	}

	sa = nullptr;
	for (auto adp : adapters)
	{
		DXGI_ADAPTER_DESC adpDesc = {};
		adp->GetDesc(&adpDesc);
		std::wstring str = adpDesc.Description;
		if (str.find(adapterName) != std::wstring::npos)
		{
			sa = adp;
			break;
		}
	}

	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	for (auto level : levels)
	{
		if (SUCCEEDED(D3D12CreateDevice(sa, level, IID_PPV_ARGS(m_device.GetAddressOf()))))
		{
			m_featureLevel = level;
			break;
		}
	}

	for (auto adp : adapters)
	{
		adp->Release();
	}

	m_rtvIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_csuIncSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return (m_device.Get() != nullptr);
}

void Renderer::enableDebugLayer()
{
	ComPtr<ID3D12Debug> debugLayer;
	D3D12GetDebugInterface(IID_PPV_ARGS(debugLayer.GetAddressOf()));
	debugLayer->EnableDebugLayer();
}

bool Renderer::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(m_cmdQueue.GetAddressOf()));
	return SUCCEEDED(hr);
}

bool Renderer::createSwapchain()
{
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width              = m_game->getWidth();
	scDesc.Height             = m_game->getHeight();
	scDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.Stereo             = FALSE;
	scDesc.BufferUsage        = DXGI_USAGE_BACK_BUFFER;
	scDesc.BufferCount        = FrameNum;
	scDesc.SampleDesc.Count   = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.Scaling            = DXGI_SCALING_NONE;
	scDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsDesc = {};
	fsDesc.Windowed = TRUE;
	HWND hwnd = m_game->getHwnd();
	ComPtr<IDXGISwapChain1> swapchain;
	HRESULT hr = m_dxgiFactory->CreateSwapChainForHwnd(
		m_cmdQueue.Get(), hwnd, &scDesc, &fsDesc,
		nullptr, swapchain.GetAddressOf());
	if (FAILED(hr)) return false;

	hr = swapchain.As(&m_swapchain);
	if (FAILED(hr)) return false;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();

	hr = m_dxgiFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

	return SUCCEEDED(hr);
}

bool Renderer::createCommandAllocators()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		HRESULT hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_cmdAllocators[i].GetAddressOf()));
		if (FAILED(hr)) return false;
	}
	return true;
}

bool Renderer::createCommandList()
{
	HRESULT hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_cmdAllocators[m_bufferIndex].Get(), nullptr,
		IID_PPV_ARGS(m_cmdList.GetAddressOf()));

	return SUCCEEDED(hr);
}

bool Renderer::createRenderTargetView()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = FrameNum;
	desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask       = 0;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()));
	if (FAILED(hr)) return false;

	auto handle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	for (UINT i = 0; i < FrameNum; ++i)
	{
		hr = m_swapchain->GetBuffer(i, IID_PPV_ARGS(m_backBuffers[i].GetAddressOf()));
		if (FAILED(hr)) return false;

		D3D12_RENDER_TARGET_VIEW_DESC rDesc = {};
		rDesc.Format               = m_renderTargetFormat;
		rDesc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
		rDesc.Texture2D.MipSlice   = 0;
		rDesc.Texture2D.PlaneSlice = 0;

		m_device->CreateRenderTargetView(m_backBuffers[i].Get(), &rDesc, handle);
		handle.ptr += m_rtvIncSize;
	}

	return true;
}

void Renderer::setBackColor(float r, float g, float b)
{
	m_backColor = XMFLOAT3(r, g, b);
}

bool Renderer::createFence()
{
	for (UINT i = 0; i < FrameNum; ++i)
	{
		m_fenceValues[i] = 0;
	}

	HRESULT hr = m_device->CreateFence(
		m_fenceValues[m_bufferIndex],
		D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.GetAddressOf()));
	if (FAILED(hr)) return false;

	m_fenceValues[m_bufferIndex]++;
	return true;
}

void Renderer::moveToNextFrame()
{
	auto currentValue = m_fenceValues[m_bufferIndex];
	m_cmdQueue->Signal(m_fence.Get(), currentValue);
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
	if (m_fence->GetCompletedValue() < m_fenceValues[m_bufferIndex])
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		if (event)
		{
			m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
			WaitForSingleObjectEx(event, INFINITE, FALSE);
			CloseHandle(event);
		}
	}
	m_fenceValues[m_bufferIndex] = currentValue + 1;
}

void Renderer::waitForGPU()
{
	m_cmdQueue->Signal(m_fence.Get(), m_fenceValues[m_bufferIndex]);
	HANDLE event = CreateEvent(nullptr, false, false, nullptr);
	if (event)
	{
		m_fence->SetEventOnCompletion(m_fenceValues[m_bufferIndex], event);
		WaitForSingleObjectEx(event, INFINITE, FALSE);
		CloseHandle(event);
	}
	m_fenceValues[m_bufferIndex]++;
	m_bufferIndex = m_swapchain->GetCurrentBackBufferIndex();
}

void Renderer::setResourceBarrier(ID3D12Resource* buffer,
	D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
	D3D12_RESOURCE_BARRIER desc = {};
	desc.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	desc.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//desc.Transition.pResource   = m_backBuffers[m_bufferIndex].Get();
	desc.Transition.pResource   = buffer;
	desc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	desc.Transition.StateBefore = stateBefore;
	desc.Transition.StateAfter  = stateAfter;

	m_cmdList->ResourceBarrier(1, &desc);
}

bool Renderer::createResourceBuffer(ID3D12Resource** buffer, UINT64 bSize)
{
	D3D12_HEAP_PROPERTIES heapProp = {};
	heapProp.Type                 = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width              = bSize;
	desc.Height             = 1;
	desc.DepthOrArraySize   = 1;
	desc.MipLevels          = 1;
	desc.Format             = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	HRESULT hr = m_device->CreateCommittedResource(
		&heapProp, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadResourceBuffer(ID3D12Resource* buffer, void* src, size_t bSize,
	void** map)
{
	if (map != nullptr)
	{
		*map = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, map);
		if (FAILED(hr) || *map == nullptr) return false;
		memcpy(*map, src, bSize);
	}
	else
	{
		void* pmap = nullptr;
		HRESULT hr = buffer->Map(0, nullptr, &pmap);
		if (FAILED(hr) || pmap == nullptr) return false;
		memcpy(pmap, src, bSize);
		buffer->Unmap(0, nullptr);
	}
	return true;
}

void Renderer::setVertexBufferView(D3D12_VERTEX_BUFFER_VIEW& vertexBufferView,
	ID3D12Resource* buffer, UINT bSize, UINT stride)
{
	vertexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes    = bSize;
	vertexBufferView.StrideInBytes  = stride;
}

void Renderer::setViewport(D3D12_VIEWPORT& viewport)
{
	viewport.Width = (float)m_game->getWidth();
	viewport.Height = (float)m_game->getHeight();
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
}

void Renderer::setScissorRect(D3D12_RECT& scissor)
{
	scissor.left = 0;
	scissor.top = 0;
	scissor.right = (LONG)m_game->getWidth();
	scissor.bottom = (LONG)m_game->getHeight();
}

void Renderer::setIndexBufferView(D3D12_INDEX_BUFFER_VIEW& indexBufferView,
	ID3D12Resource* buffer, UINT bSize)
{
	indexBufferView.BufferLocation = buffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = bSize;
	indexBufferView.Format = DXGI_FORMAT_R16_UINT;
}

bool Renderer::createShaderResource(ID3D12Resource** buffer, UINT width, UINT height,
	DXGI_FORMAT format, UINT16 mipLevels, UINT16 depthOrArraySize,
	D3D12_RESOURCE_DIMENSION dimension, D3D12_CLEAR_VALUE* clearValue)
{
	D3D12_HEAP_PROPERTIES prop = {};
	if (clearValue == nullptr)
	{
		prop.Type                 = D3D12_HEAP_TYPE_CUSTOM;
		prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	}
	else
	{
		prop.Type                 = D3D12_HEAP_TYPE_DEFAULT;
		prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	}

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension          = dimension;
	desc.Width              = width;
	desc.Height             = height;
	desc.DepthOrArraySize   = depthOrArraySize;
	desc.MipLevels          = mipLevels;
	desc.Format             = format;
	desc.SampleDesc.Count   = 1;
	desc.SampleDesc.Quality = 0;
	//desc.Flags              = D3D12_RESOURCE_FLAG_NONE;
	desc.Flags = (clearValue == nullptr) ? D3D12_RESOURCE_FLAG_NONE
		: D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	desc.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	HRESULT hr = m_device->CreateCommittedResource(
		&prop, D3D12_HEAP_FLAG_NONE,
		&desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		clearValue, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr);
}

bool Renderer::uploadShaderResource(ID3D12Resource* buffer, void* src,
	UINT lineSize, UINT allSize)
{
	HRESULT hr = buffer->WriteToSubresource(0, nullptr,
		src, lineSize, allSize);

	return SUCCEEDED(hr);
}

bool Renderer::createDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask       = 0;
	desc.NumDescriptors = dNum;
	desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(dHeap));

	return SUCCEEDED(hr);
}

void Renderer::setShaderResourceView(ID3D12Resource* buffer, DXGI_FORMAT format,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format                  = format;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension           = D3D12_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels     = 1;

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_csuIncSize * index;
	m_device->CreateShaderResourceView(buffer, &desc, handle);
}

bool Renderer::readImageFile(ID3D12Resource** buffer, TexMetadata& metadata,
	const wchar_t* filePath, bool ddsFlag)
{
	ScratchImage img = {};

	HRESULT hr;
	if (ddsFlag)
	{
		hr = LoadFromDDSFile(filePath, DDS_FLAGS_NONE, &metadata, img);
	}
	else
	{
		hr = LoadFromWICFile(filePath, WIC_FLAGS_NONE, &metadata, img);
	}
	if (FAILED(hr)) return false;

	if (!createShaderResource(buffer, (UINT)metadata.width,
		(UINT)metadata.height, metadata.format,
		(UINT16)metadata.mipLevels, (UINT16)metadata.arraySize,
		(D3D12_RESOURCE_DIMENSION)metadata.dimension)) return false;

	auto image = img.GetImage(0, 0, 0);
	if (!uploadShaderResource(*buffer, image->pixels, (UINT)image->rowPitch,
		(UINT)image->slicePitch)) return false;

	return true;
}

bool Renderer::createConstBuffer(ID3D12Resource** buffer, const void* src,
	size_t dsize, void** pmap)
{
	if (!createResourceBuffer(buffer, calcAlignment256(dsize))) return false;

	if (pmap == nullptr)
	{
		void* map = nullptr;
		HRESULT hr = (*buffer)->Map(0, nullptr, &map);
		if (FAILED(hr) || map == nullptr) return false;
		memcpy(map, src, dsize);
		(*buffer)->Unmap(0, nullptr);
	}
	else
	{
		*pmap = nullptr;
		HRESULT hr = (*buffer)->Map(0, nullptr, pmap);
		if (FAILED(hr) || (*pmap) == nullptr) return false;
		memcpy(*pmap, src, dsize);
	}

	return true;
}

void Renderer::setConstBufferView(ID3D12Resource* buffer,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	D3D12_CONSTANT_BUFFER_VIEW_DESC desc = {};
	desc.BufferLocation = buffer->GetGPUVirtualAddress();
	desc.SizeInBytes = (UINT)buffer->GetDesc().Width;

	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * m_csuIncSize;

	m_device->CreateConstantBufferView(&desc, handle);
}

UINT64 Renderer::calcAlignment256(size_t size)
{
	UINT64 s = (UINT64)size;

	return (size + 0xff) & ~0xff;
}

void Renderer::drawBatchSprite()
{
	for (size_t i = 0; i < m_spriteNum; ++i)
	{
		setShader(m_spriteDrawList[i].shaderIndex);
		setCommandCSBufferView(m_spriteDrawList[i].modelIndex);
		m_cmdList->DrawInstanced(4, 1, 0, 0);
	}
}

void Renderer::setCameraInParam(CamInParam inParam)
{
	m_camera.setInParam(inParam);
	m_cameraMatrix = m_camera.calcViewProjMatrix(
		(float)m_game->getWidth(), (float)m_game->getHeight());
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);
}

void Renderer::setCameraExtParam(CamExtParam extParam)
{
	m_camera.setExtParam(extParam);
	m_cameraMatrix = m_camera.calcViewProjMatrix(
		(float)m_game->getWidth(), (float)m_game->getHeight());
	m_cameraViewMatrix = m_camera.calcViewMatrix();
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);
}

void Renderer::setCameraParam(CamInParam inParam, CamExtParam extParam)
{
	m_camera.setInParam(inParam);
	m_camera.setExtParam(extParam);
	m_cameraMatrix = m_camera.calcViewProjMatrix(
		(float)m_game->getWidth(), (float)m_game->getHeight());
	m_cameraViewMatrix = m_camera.calcViewMatrix();
	m_cameraInverseMatrix = XMMatrixInverse(nullptr, m_cameraMatrix);
}

int Renderer::allocateConstBuffer(const void* pData, size_t dsize, bool meshFlag)
{
	int index = 0;
	int maxIndex = MaxSpriteNum;
	if (meshFlag)
	{
		index    = MaxSpriteNum;
		maxIndex = MaxObjectNum;
	}
	for (; index < maxIndex; ++index)
	{
		if (m_constBuffer[0][index].Get() == nullptr) break;
	}
	if (index >= maxIndex) return -1;

	for (int i = 0; i < FrameNum; ++i)
	{
		if (!createConstBuffer(m_constBuffer[i][index].GetAddressOf(),
			pData, dsize, &m_constBufferMap[i][index]))
		{
			if (i > 0)
			{
				for (int j = i - 1; j >= 0; --j)
				{
					m_constBuffer[j][index].Reset();
				}
			}
			return -1;
		}

		setConstBufferView(m_constBuffer[i][index].Get(), m_scDescHeap[i].Get(),
			index * ViewNum);
		for (int k = 0; k < TexViewNum; ++k)
		{
			setShaderResourceView(nullptr, DXGI_FORMAT_R8G8B8A8_UNORM,
				m_scDescHeap[i].Get(), index * ViewNum + ConstViewNum + k);
		}
	}
	return index;
}

void Renderer::uploadConstBuffer(int index, const void* pData, size_t dsize)
{
	if (index < 0 || index >= MaxObjectNum) return;

	memcpy(m_constBufferMap[m_bufferIndex][index], pData, dsize);
}

void Renderer::releaseConstBuffer(int index)
{
	if (index < 0 || index >= MaxObjectNum) return;
	if (m_constBuffer[0][index].Get() == nullptr) return;

	m_constBufferReleaseList.push_back(std::make_pair(index, ReleaseCountStart));
}

void Renderer::setCommandCSBufferView(int index)
{
	if (index < 0 || index >= MaxObjectNum) return;

	int idx = ViewNum * index;

	{
		// 定数バッファビュー
		auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += idx * m_csuIncSize;
		m_cmdList->SetGraphicsRootDescriptorTable(1, handle);
	}
	{
		// シェーダリソースビュー
		auto handle = m_scDescHeap[m_bufferIndex]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += (idx + ConstViewNum) * m_csuIncSize;
		m_cmdList->SetGraphicsRootDescriptorTable(0, handle);
	}
}

ImageData Renderer::allocateShaderResource(const std::wstring& filePath,
	bool ddsFlag)
{
	auto it = m_textures.find(filePath);
	if (it != m_textures.end()) return (*it).second;

	ImageData imgData;
	if (m_textureNum >= MaxTextureNum) return imgData;

	int index = 0;
	while (index < MaxTextureNum)
	{
		if (m_textureBuffer[index].Get() == nullptr) break;
		++index;
	}
	if (index >= MaxTextureNum) return imgData;

	TexMetadata metadata = {};
	if (!readImageFile(m_textureBuffer[index].GetAddressOf(),
		metadata, filePath.c_str(), ddsFlag)) return imgData;

	imgData.imgIndex = index;
	imgData.width = (int)metadata.width;
	imgData.height = (int)metadata.height;
	imgData.filePath = filePath;
	imgData.format = metadata.format;
	m_textures[filePath] = imgData;

	return imgData;
}

void Renderer::setMaterialSlot(int index, int slotNum, const ImageData& imgData)
{
	if (index < 0 || index >= MaxObjectNum) return;
	if (slotNum < 0 || slotNum >= TexViewNum) return;

	for (int i = 0; i < FrameNum; ++i)
	{
		setShaderResourceView(m_textureBuffer[imgData.imgIndex].Get(),
			imgData.format, m_scDescHeap[i].Get(), index * ViewNum + ConstViewNum + slotNum);
	}
}

bool Renderer::drawSprite(int modelIndex, int shaderIndex, 
	ImageData imgData, XMFLOAT2 pos, float theta,
	XMFLOAT2* scale, XMFLOAT2 offset,
	XMFLOAT2 imgPos, XMFLOAT2* imgScale, 
	XMFLOAT3 color, float alpha)
{
	if (m_spriteNum >= MaxSpriteNum) return false;

	XMFLOAT2 texSize = (scale != nullptr) ? *scale
		: XMFLOAT2((float)imgData.width, (float)imgData.height);
	XMFLOAT2 uvSize = (imgScale != nullptr) ? *imgScale
		: XMFLOAT2((float)imgData.width, (float)imgData.height);

	SpriteTransData mat;
	mat.posMat = 
		calcSpriteModelMatrix(texSize, pos, offset, theta) * m_spriteMatrix;
	mat.uvMat = calcSpriteUVMatrix(imgPos, uvSize.x, uvSize.y,
		(float)imgData.width, (float)imgData.height);
	mat.color = color;
	mat.alpha = alpha;

	memcpy(m_constBufferMap[m_bufferIndex][modelIndex], (void*)&mat, 
		sizeof(SpriteTransData));
	m_spriteDrawList[m_spriteNum] = SpriteDrawInfo(modelIndex, shaderIndex);

	m_spriteNum += 1;
	return true;
}

void Renderer::setShader(int shaderIndex)
{
	if (m_shaderIndex == shaderIndex) return;
	if (shaderIndex < 0 || shaderIndex >= ShaderNum) return;

	m_shaders[shaderIndex]->setShader(m_cmdList.Get());
	m_shaderIndex = shaderIndex;
}

ImageData Renderer::createUnicolorTexture(const wchar_t* imageName, ColorRGBA color)
{
	ImageData imgData;
	if (m_textureNum >= MaxTextureNum) return imgData;
	auto it = m_textures.find(imageName);
	if (it != m_textures.end()) return imgData;

	int index = 0;
	while (index < MaxTextureNum)
	{
		if (m_textureBuffer[index].Get() == nullptr) break;
		++index;
	}
	if (index >= MaxTextureNum) return imgData;

	std::vector<ColorRGBA> colors(4 * 4);
	for (ColorRGBA& c : colors)
	{
		c = color;
	}
	if (!createShaderResource(m_textureBuffer[index].GetAddressOf(),
		4, 4, DXGI_FORMAT_R8G8B8A8_UNORM)) return imgData;
	if (!uploadShaderResource(m_textureBuffer[index].Get(), colors.data(),
		(UINT)(sizeof(ColorRGBA) * 4), (UINT)(sizeof(ColorRGBA) * 16))) return imgData;

	imgData.imgIndex = index;
	imgData.width = 4;
	imgData.height = 4;
	imgData.filePath = imageName;
	imgData.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_textures[imageName] = imgData;

	return imgData;
}

bool Renderer::drawMesh(int shaderIndex, int modelIndex, MeshData meshData,
	int indicesNum, int startPos, int instanceNum)
{
	if (m_meshNum >= MaxMeshNum) return false;

	m_meshDrawList[m_meshNum] = MeshDrawInfo(modelIndex, shaderIndex,
		meshData, indicesNum, startPos, instanceNum);
	m_meshNum += 1;

	return true;
}

MeshData Renderer::getMeshData(const wchar_t* meshName)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return ((*it).second)[0].mesh;

	return MeshData(-1, 0, 0);
}

bool Renderer::loadGmesh(const wchar_t* filePath, std::vector<BufferData>& data)
{
	auto it = m_meshData.find(filePath);
	if (it != m_meshData.end())
	{
		data.clear();
		data = (*it).second;
		return true;
	}

	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	// メッシュの個数
	unsigned int meshNum;
	fin.read((char*)&meshNum, sizeof(unsigned int));
	if (fin.gcount() != sizeof(unsigned int)) return false;

	std::vector<std::vector<VertexNorm>>     vertices(meshNum);
	std::vector<std::vector<unsigned short>> indices(meshNum);
	std::vector<std::vector<wchar_t>>        texPath(meshNum);
	std::vector<XMMATRIX>                    adjust(meshNum);

	// メッシュデータの取得
	for (unsigned int i = 0; i < meshNum; ++i)
	{
		// 頂点の個数
		unsigned int vertexNum;
		fin.read((char*)&vertexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;
		// インデックスの個数
		unsigned int indexNum;
		fin.read((char*)&indexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;
		// 頂点データの読込み
		vertices[i].resize(vertexNum);
		fin.read((char*)vertices[i].data(), sizeof(VertexNorm) * vertexNum);
		if (fin.gcount() != sizeof(VertexNorm) * vertexNum) return false;
		// インデックスデータの読込み
		indices[i].resize(indexNum);
		fin.read((char*)indices[i].data(), sizeof(unsigned short) * indexNum);
		if (fin.gcount() != sizeof(unsigned short) * indexNum) return false;
		// テクスチャのファイル名取得
		texPath[i].resize(256);
		fin.read((char*)texPath[i].data(), sizeof(wchar_t) * 256);
		if (fin.gcount() != sizeof(wchar_t) * 256) return false;
		// 調整用変換行列の取得
		std::vector<float> ad(16);
		fin.read((char*)ad.data(), sizeof(float) * 16);
		if (fin.gcount() != sizeof(float) * 16) return false;
		setContainerToMatrix(ad, adjust[i]);
	}
	fin.close();

	// データ生成
	std::vector<BufferData> bdata(meshNum);
	std::vector<int> mindex;
	for (unsigned int i = 0; i < meshNum; ++i)
	{
		int index = 0;
		for (; index < MaxMeshDataNum; ++index)
		{
			if (m_vertexBuffer[index].Get() == nullptr) break;
		}
		if (index >= MaxMeshDataNum) return false;
		mindex.push_back(index);

		// 頂点バッファ
		if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
			vertices[i].size() * sizeof(VertexNorm)))
		{
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
			(void*)vertices[i].data(), vertices[i].size() * sizeof(VertexNorm)))
		{
			m_vertexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		setVertexBufferView(m_vertexBufferView[index],
			m_vertexBuffer[index].Get(), (UINT)(vertices[i].size() * sizeof(VertexNorm)),
			sizeof(VertexNorm));

		// インデックスバッファ
		if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
			indices[i].size() * sizeof(unsigned short)))
		{
			m_vertexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
			(void*)indices[i].data(), indices[i].size() * sizeof(unsigned short)))
		{
			m_vertexBuffer[index].Reset();
			m_indexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		setIndexBufferView(m_indexBufferView[index],
			m_indexBuffer[index].Get(), (UINT)(indices[i].size() * sizeof(unsigned short)));

		ImageData imgData = allocateShaderResource(L"src\\" + std::wstring(texPath[i].data()));
		if (imgData.imgIndex == -1)
		{
			imgData = allocateShaderResource(WhiteTexture);
		}

		bdata[i].mesh = MeshData(index, (int)vertices[i].size(), (int)indices[i].size());
		bdata[i].texture = imgData;
		bdata[i].adjust = adjust[i];
	}

	m_meshData[filePath] = bdata;
	data.clear();
	data = bdata;

	return true;
}

bool Renderer::loadGanim(const wchar_t* filePath, std::vector<BufferData>& data)
{
	auto it = m_meshData.find(filePath);
	if (it != m_meshData.end())
	{
		data.clear();
		data = (*it).second;
		return true;
	}

	std::ifstream fin(filePath, std::ios::in | std::ios::binary);
	if (!fin) return false;

	// メッシュの個数
	unsigned int meshNum;
	fin.read((char*)&meshNum, sizeof(unsigned int));
	if (fin.gcount() != sizeof(unsigned int)) return false;

	std::vector<std::vector<VertexAnim>>     vertices(meshNum);
	std::vector<std::vector<unsigned short>> indices(meshNum);
	std::vector<AnimData>							animData(meshNum);
	std::vector<std::vector<XMMATRIX>>              inverseMatrices(meshNum);
	std::vector<std::vector<std::vector<XMMATRIX>>> poseMatrices(meshNum);
	std::vector<std::vector<wchar_t>>        texPath(meshNum);
	std::vector<XMMATRIX>                    adjust(meshNum);

	// メッシュデータの取得
	for (unsigned int i = 0; i < meshNum; ++i)
	{
		// 頂点の個数
		unsigned int vertexNum;
		fin.read((char*)&vertexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;
		// インデックスの個数
		unsigned int indexNum;
		fin.read((char*)&indexNum, sizeof(unsigned int));
		if (fin.gcount() != sizeof(unsigned int)) return false;
		// 頂点データの読込み
		vertices[i].resize(vertexNum);
		fin.read((char*)vertices[i].data(), sizeof(VertexAnim) * vertexNum);
		if (fin.gcount() != sizeof(VertexAnim) * vertexNum) return false;
		// インデックスデータの読込み
		indices[i].resize(indexNum);
		fin.read((char*)indices[i].data(), sizeof(unsigned short) * indexNum);
		if (fin.gcount() != sizeof(unsigned short) * indexNum) return false;

		// アニメーションデータの読込み
		fin.read((char*)&animData[i], sizeof(AnimData));
		if (fin.gcount() != sizeof(AnimData)) return false;
		int bnum = animData[i].boneNum;
		int fnum = animData[i].frameNum;
		int anum = bnum * fnum;
		inverseMatrices[i].resize(bnum);
		poseMatrices[i].resize(fnum);
		for (size_t j = 0; j < poseMatrices[i].size(); ++j)
		{
			poseMatrices[i][j].resize(bnum);
		}
		std::vector<float> im(16 * bnum);
		std::vector<float> pm(16 * anum);
		fin.read((char*)im.data(), sizeof(float) * 16 * bnum);
		if (fin.gcount() != sizeof(float) * 16 * bnum) return false;
		fin.read((char*)pm.data(), sizeof(float) * 16 * anum);
		if (fin.gcount() != sizeof(float) * 16 * anum) return false;
		setContainerToMatrices(im, inverseMatrices[i]);
		setContainerToMultiMatrices(pm, poseMatrices[i]);

		// テクスチャのファイル名取得
		texPath[i].resize(256);
		fin.read((char*)texPath[i].data(), sizeof(wchar_t) * 256);
		if (fin.gcount() != sizeof(wchar_t) * 256) return false;
		// 調整用変換行列の取得
		std::vector<float> ad(16);
		fin.read((char*)ad.data(), sizeof(float) * 16);
		if (fin.gcount() != sizeof(float) * 16) return false;
		setContainerToMatrix(ad, adjust[i]);
	}
	fin.close();

	// データ生成
	std::vector<BufferData> bdata(meshNum);
	std::vector<int> mindex;
	for (unsigned int i = 0; i < meshNum; ++i)
	{
		int index = 0;
		for (; index < MaxMeshDataNum; ++index)
		{
			if (m_vertexBuffer[index].Get() == nullptr) break;
		}
		if (index >= MaxMeshDataNum) return false;
		mindex.push_back(index);

		// 頂点バッファ
		if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
			vertices[i].size() * sizeof(VertexAnim)))
		{
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
			(void*)vertices[i].data(), vertices[i].size() * sizeof(VertexAnim)))
		{
			m_vertexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		setVertexBufferView(m_vertexBufferView[index],
			m_vertexBuffer[index].Get(), (UINT)(vertices[i].size() * sizeof(VertexAnim)),
			sizeof(VertexAnim));

		// インデックスバッファ
		if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
			indices[i].size() * sizeof(unsigned short)))
		{
			m_vertexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
			(void*)indices[i].data(), indices[i].size() * sizeof(unsigned short)))
		{
			m_vertexBuffer[index].Reset();
			m_indexBuffer[index].Reset();
			for (size_t j = 0; j < mindex.size(); ++j)
			{
				m_vertexBuffer[mindex[j]].Reset();
				m_indexBuffer[mindex[j]].Reset();
			}
			return false;
		}
		setIndexBufferView(m_indexBufferView[index],
			m_indexBuffer[index].Get(), (UINT)(indices[i].size() * sizeof(unsigned short)));

		ImageData imgData = allocateShaderResource(L"src\\" + std::wstring(texPath[i].data()));
		if (imgData.imgIndex == -1)
		{
			imgData = allocateShaderResource(WhiteTexture);
		}

		bdata[i].mesh = MeshData(index, (int)vertices[i].size(), (int)indices[i].size());
		bdata[i].animData = animData[i];
		bdata[i].inverseGPoseMatrices = inverseMatrices[i];
		bdata[i].poseMatrices = poseMatrices[i];
		bdata[i].texture = imgData;
		bdata[i].adjust = adjust[i];
	}

	m_meshData[filePath] = bdata;
	data.clear();
	data = bdata;

	return true;
}

bool Renderer::createDepthBuffer(ID3D12Resource** buffer)
{
	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension        = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width            = (UINT64)m_game->getWidth();
	desc.Height           = (UINT64)m_game->getHeight();
	desc.DepthOrArraySize = 1;
	desc.Format           = DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count = 1;
	desc.Flags            = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES prop = {};
	prop.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	prop.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_CLEAR_VALUE value = {};
	value.DepthStencil.Depth = 1.0f;
	value.Format             = DXGI_FORMAT_D32_FLOAT;

	HRESULT hr = m_device->CreateCommittedResource(&prop,
		D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&value, IID_PPV_ARGS(buffer));

	return SUCCEEDED(hr) ? true : false;
}

bool Renderer::createDepthHeap(ID3D12DescriptorHeap** heap, ID3D12Resource* depth)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.NumDescriptors = 1;
	desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;

	HRESULT hr = m_device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(heap));
	if (FAILED(hr)) return false;

	D3D12_DEPTH_STENCIL_VIEW_DESC  dsvDesc = {};
	dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;

	m_device->CreateDepthStencilView(depth, &dsvDesc,
		(*heap)->GetCPUDescriptorHandleForHeapStart());

	return true;
}

bool Renderer::createPlaneMesh(const wchar_t* meshName, float width, int xNum, int yNum,
	bool tangentFlag)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return false;

	int index = 0;
	for (; index < MaxMeshDataNum; ++index)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxMeshDataNum) return false;

	int vsize = 0;
	if (tangentFlag == false)
	{
		std::vector<VertexNorm> vertices(xNum * yNum);
		for (int x = 0; x < xNum; ++x)
		{
			for (int y = 0; y < yNum; ++y)
			{
				int index = y + x * yNum;
				XMFLOAT3 p = { 0.5f * width - (float)x * (width / (float)(xNum - 1)),
					-0.5f * width + (float)y * (width / (float)(yNum - 1)), 0.0f };
				XMFLOAT2 uv = { (float)y / (float)(yNum - 1), (float)x / (float)(xNum - 1) };
				vertices[index] = { p, uv, UnitVecZ3d };
			}
		}
		if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
			vertices.size() * sizeof(VertexNorm))) return false;
		if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
			(void*)vertices.data(), vertices.size() * sizeof(VertexNorm)))
		{
			m_vertexBuffer[index].Reset();
			return false;
		}
		setVertexBufferView(m_vertexBufferView[index],
			m_vertexBuffer[index].Get(),
			(UINT)vertices.size() * sizeof(VertexNorm), sizeof(VertexNorm));
		vsize = (int)vertices.size();
	}
	else
	{
		std::vector<VertexNormTangent> vertices(xNum * yNum);
		for (int x = 0; x < xNum; ++x)
		{
			for (int y = 0; y < yNum; ++y)
			{
				int index = y + x * yNum;
				XMFLOAT3 p = { 0.5f * width - (float)x * (width / (float)(xNum - 1)),
					-0.5f * width + (float)y * (width / (float)(yNum - 1)), 0.0f };
				XMFLOAT2 uv = { (float)y / (float)(yNum - 1), (float)x / (float)(xNum - 1) };
				vertices[index] = { p, uv, UnitVecZ3d, UnitVecY3d };
			}
		}
		if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
			vertices.size() * sizeof(VertexNormTangent))) return false;
		if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
			(void*)vertices.data(), vertices.size() * sizeof(VertexNormTangent)))
		{
			m_vertexBuffer[index].Reset();
			return false;
		}
		setVertexBufferView(m_vertexBufferView[index],
			m_vertexBuffer[index].Get(),
			(UINT)vertices.size() * sizeof(VertexNormTangent), sizeof(VertexNormTangent));
		vsize = (int)vertices.size();
	}

	int polyX = xNum - 1;
	int polyY = yNum - 1;
	int polyNum = polyX * polyY;
	std::vector<unsigned short> indices(polyNum * 6);
	for (int x = 0; x < polyX; ++x)
	{
		for (int y = 0; y < polyY; ++y)
		{
			int v0 = y + x * yNum;
			int v1 = v0 + 1;
			int v2 = v0 + yNum;
			int v3 = v2 + 1;
			int index = 6 * (y + x * polyY);
			indices[index] = v0;
			indices[index + 1] = v3;
			indices[index + 2] = v2;
			indices[index + 3] = v0;
			indices[index + 4] = v1;
			indices[index + 5] = v3;
		}
	}
	if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		indices.size() * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
		(void*)indices.data(), indices.size() * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		m_indexBuffer[index].Reset();
		return false;
	}
	setIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), (UINT)indices.size() * sizeof(unsigned short));

	std::vector<BufferData> data(1);
	data[0].mesh = MeshData(index, vsize, (int)indices.size());
	m_meshData[meshName] = data;

	return true;
}

bool Renderer::createCubeMesh(const wchar_t* meshName, bool outer, float width)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return false;

	int index = 0;
	for (; index < MaxMeshDataNum; ++index)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxMeshDataNum) return false;

	VertexNorm vertices[24] = {};
	float sign = (outer) ? 1.0f : -1.0f;
	XMFLOAT2 uv[4];
	uv[0] = { 0.0f, 1.0f };
	uv[1] = { 0.0f, 0.0f };
	uv[2] = { 1.0f, 1.0f };
	uv[3] = { 1.0f, 0.0f };

	// x-front
	XMFLOAT3 norm = outer * UnitVecX3d;
	vertices[0] = { { 0.5f * width, sign *  0.5f * width, -0.5f * width }, uv[0], norm };
	vertices[1] = { { 0.5f * width, sign *  0.5f * width,  0.5f * width }, uv[1], norm };
	vertices[2] = { { 0.5f * width, sign * -0.5f * width, -0.5f * width }, uv[2], norm };
	vertices[3] = { { 0.5f * width, sign * -0.5f * width,  0.5f * width }, uv[3], norm };

	// y-front
	norm = outer * UnitVecY3d;
	vertices[4] = { { sign * -0.5f * width, 0.5f * width, -0.5f * width }, uv[0], norm };
	vertices[5] = { { sign * -0.5f * width, 0.5f * width,  0.5f * width }, uv[1], norm };
	vertices[6] = { { sign *  0.5f * width, 0.5f * width, -0.5f * width }, uv[2], norm };
	vertices[7] = { { sign *  0.5f * width, 0.5f * width,  0.5f * width }, uv[3], norm };

	// x-back
	norm = -1.0f * outer * UnitVecX3d;
	vertices[ 8] = { { -0.5f * width, sign * -0.5f * width, -0.5f * width }, uv[0], norm };
	vertices[ 9] = { { -0.5f * width, sign * -0.5f * width,  0.5f * width }, uv[1], norm };
	vertices[10] = { { -0.5f * width, sign *  0.5f * width, -0.5f * width }, uv[2], norm };
	vertices[11] = { { -0.5f * width, sign *  0.5f * width,  0.5f * width }, uv[3], norm };

	// y-back
	norm = -1.0f * outer * UnitVecY3d;
	vertices[12] = { { sign *  0.5f * width, -0.5f * width, -0.5f * width }, uv[0], norm };
	vertices[13] = { { sign *  0.5f * width, -0.5f * width,  0.5f * width }, uv[1], norm };
	vertices[14] = { { sign * -0.5f * width, -0.5f * width, -0.5f * width }, uv[2], norm };
	vertices[15] = { { sign * -0.5f * width, -0.5f * width,  0.5f * width }, uv[3], norm };

	// z-front
	norm = outer * UnitVecZ3d;
	vertices[16] = { {  0.5f * width, sign *  0.5f * width, 0.5f * width }, uv[0], norm };
	vertices[17] = { { -0.5f * width, sign *  0.5f * width, 0.5f * width }, uv[1], norm };
	vertices[18] = { {  0.5f * width, sign * -0.5f * width, 0.5f * width }, uv[2], norm };
	vertices[19] = { { -0.5f * width, sign * -0.5f * width, 0.5f * width }, uv[3], norm };

	// z-back
	norm = -1.0f * outer * UnitVecZ3d;
	vertices[20] = { { -0.5f * width, sign *  0.5f * width, -0.5f * width }, uv[0], norm };
	vertices[21] = { {  0.5f * width, sign *  0.5f * width, -0.5f * width }, uv[1], norm };
	vertices[22] = { { -0.5f * width, sign * -0.5f * width, -0.5f * width }, uv[2], norm };
	vertices[23] = { {  0.5f * width, sign * -0.5f * width, -0.5f * width }, uv[3], norm };

	if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
		24 * sizeof(VertexNorm))) return false;
	if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
		(void*)vertices, 24 * sizeof(VertexNorm)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	setVertexBufferView(m_vertexBufferView[index],
		m_vertexBuffer[index].Get(), 24 * sizeof(VertexNorm), sizeof(VertexNorm));

	unsigned short indices[36] = {};
	for (int i = 0; i < 6; ++i)
	{
		unsigned short index = 4 * i;
		unsigned short s = 1;
		for (int j = 0; j < 6; ++j)
		{
			indices[6 * i + j] = index;
			index += s;
			if (j == 2)
			{
				index = 4 * i + 3;
				s = -1;
			}
		}
	}

	if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		36 * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
		(void*)indices, 36 * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		m_indexBuffer[index].Reset();
		return false;
	}
	setIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), 36 * sizeof(unsigned short));

	std::vector<BufferData> data(1);
	data[0].mesh = MeshData(index, 24, 36);
	m_meshData[meshName] = data;

	return true;
}

bool Renderer::createHemisphereMesh(const wchar_t* meshName, bool outer, float radius,
	UINT nv, UINT nh, float cu, float cv, float uvR)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return false;

	int index = 0;
	for (; index < MaxMeshDataNum; ++index)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxMeshDataNum) return false;

	nv = (nv >= 1) ? nv : 1;
	nh = (nh >= 3) ? nh : 3;
	float angleV = XM_PIDIV2 / nv;
	float sign = (outer) ? -1.0f : 1.0f;
	float angleH = sign * XM_2PI / nh;

	int vnum = nv * (nh + 1) + 1;
	std::vector<VertexNorm> vertices(vnum);
	int pos = 0;
	for (UINT i = 0; i <= nv; ++i)
	{
		if (i == 0)
		{
			vertices[pos++] = { {0.0f, 0.0f, radius }, { cu, cv }, -1.0f * sign * UnitVecZ3d };
		}
		else
		{
			float rh = radius * XMScalarSin(i * angleV);
			float z = radius * XMScalarCos(i * angleV);
			for (UINT j = 0; j <= nh; ++j)
			{
				XMFLOAT3 p = { rh * XMScalarCos(j * angleH), 
					rh * XMScalarSin(j * angleH), z };
				float phi = i * angleV;
				float proj = XMScalarSin(phi);
				XMFLOAT2 uv = { uvR * proj * XMScalarCos(-(j * angleH)) + cu,
					uvR * proj * XMScalarSin(-(j * angleH)) + cv };
				XMFLOAT3 norm = -1.0f * sign * normalize(p);
				vertices[pos++] = { p, uv, norm };
			}
		}
	}

	if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
		vnum * sizeof(VertexNorm))) return false;
	if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
		(void*)vertices.data(), vnum * sizeof(VertexNorm)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	setVertexBufferView(m_vertexBufferView[index],
		m_vertexBuffer[index].Get(), vnum * sizeof(VertexNorm), sizeof(VertexNorm));

	int inum = 3 * nh * (2 * nv - 1);
	pos = 0;
	std::vector<unsigned short> indices(inum);
	for (unsigned short i = 1; i <= nh; ++i)
	{
		indices[pos++] = i;
		indices[pos++] = 0;
		indices[pos++] = i + 1;
	}
	for (unsigned short i = 1; i <= (nv - 1); ++i)
	{
		unsigned short baseV = 1 + (i - 1) * (nh + 1);
		for (unsigned short j = 1; j <= nh; ++j)
		{
			unsigned short baseH = baseV + j - 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + nh + 2;
			indices[pos++] = baseH + nh + 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + 1;
			indices[pos++] = baseH + nh + 2;
		}
	}

	if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		inum * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
		(void*)indices.data(), inum * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		m_indexBuffer[index].Reset();
		return false;
	}
	setIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), inum * sizeof(unsigned short));

	std::vector<BufferData> data(1);
	data[0].mesh = MeshData(index, vnum, inum);
	m_meshData[meshName] = data;

	return true;
}

bool Renderer::createSphereMesh(const wchar_t* meshName, bool outer, float radius,
	UINT nv, UINT nh)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return false;

	int index = 0;
	for (; index < MaxMeshDataNum; ++index)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxMeshDataNum) return false;

	nv = (nv >= 2) ? nv : 2;
	nh = (nh >= 3) ? nh : 3;
	float angleV = XM_PI / (float)nv;
	float sign = (outer) ? -1.0f : 1.0f;
	float angleH = sign * XM_2PI / (float)nh;
	float stepU = 1.0f / (float)nh;
	float stepV = 1.0f / (float)nv;

	int vnum = (nv - 1) * (nh + 1) + 2;
	std::vector<VertexNorm> vertices(vnum);
	int pos = 0;
	for (UINT i = 0; i <= nv; ++i)
	{
		if (i == 0)
		{
			vertices[pos++] = { { 0.0f, 0.0f, radius }, ZeroVec2d, -1.0f * sign * UnitVecZ3d };
		}
		else if (i < nv)
		{
			float rh = radius * XMScalarSin(i * angleV);
			float z = radius * XMScalarCos(i * angleV);
			for (UINT j = 0; j <= nh; ++j)
			{
				XMFLOAT3 p = { rh * XMScalarCos(j * angleH), rh * XMScalarSin(j * angleH), z };
				XMFLOAT3 norm = -1.0f * sign * normalize(p);
				vertices[pos++] = { p, { stepU * j, stepV * i }, norm };
			}
		}
		else
		{
			vertices[pos++] = { { 0.0f, 0.0f, -radius }, { 0.0f, 1.0f }, sign * UnitVecZ3d };
		}
	}

	if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
		vnum * sizeof(VertexNorm))) return false;
	if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
		(void*)vertices.data(), vnum * sizeof(VertexNorm)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	setVertexBufferView(m_vertexBufferView[index],
		m_vertexBuffer[index].Get(), vnum * sizeof(VertexNorm), sizeof(VertexNorm));

	int inum = 6 * nh * (nv - 1);
	pos = 0;
	unsigned short sIndex = 0;
	unsigned short eIndex = vnum - 1;
	std::vector<unsigned short> indices(inum);
	for (unsigned short i = 1; i <= nh; ++i)
	{
		indices[pos++] = i;
		indices[pos++] = sIndex;
		indices[pos++] = i + 1;
	}
	for (unsigned short i = 1; i <= (nv - 2); ++i)
	{
		unsigned short baseV = 1 + (i - 1) * (nh + 1);
		for (unsigned short j = 1; j <= nh; ++j)
		{
			unsigned short baseH = baseV + j - 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + nh + 2;
			indices[pos++] = baseH + nh + 1;
			indices[pos++] = baseH;
			indices[pos++] = baseH + 1;
			indices[pos++] = baseH + nh + 2;
		}
	}
	unsigned short base = 1 + (nv - 2) * (nh + 1);
	for (unsigned short i = 1; i <= nh; ++i)
	{
		indices[pos++] = base + i - 1;
		indices[pos++] = base + i;
		indices[pos++] = eIndex;
	}

	if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		inum * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		return false;
	}
	if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
		(void*)indices.data(), inum * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		m_indexBuffer[index].Reset();
		return false;
	}
	setIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), inum * sizeof(unsigned short));

	std::vector<BufferData> data(1);
	data[0].mesh = MeshData(index, vnum, inum);
	m_meshData[meshName] = data;

	return true;
}

void Renderer::drawBatchMesh()
{
	for (size_t i = 0; i < m_meshNum; ++i)
	{
		int meshIndex = m_meshDrawList[i].meshData.meshIndex;
		setShader(m_meshDrawList[i].shaderIndex);
		setCommandCSBufferView(m_meshDrawList[i].modelIndex);
		m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView[meshIndex]);
		m_cmdList->IASetIndexBuffer(&m_indexBufferView[meshIndex]);
		m_cmdList->DrawIndexedInstanced(m_meshDrawList[i].indicesNum,
			m_meshDrawList[i].instanceNum, m_meshDrawList[i].startPos, 0, 0);
	}
}

void Renderer::drawBatchMeshTranslucent()
{
	if (m_meshTranslucentNum <= 0) return;

	std::sort(m_meshTranslucentIndex.begin(),
		m_meshTranslucentIndex.begin() + m_meshTranslucentNum,
		[](const std::pair<float, int>& v1, const std::pair<float, int>& v2)
		{ return v1.first > v2.first; });

	for (int i = 0; i < m_meshTranslucentNum; ++i)
	{
		int index = m_meshTranslucentIndex[i].second;
		MeshDrawInfo info = m_meshDrawTranslucentList[index];
		int meshIndex = info.meshData.meshIndex;
		setShader(info.shaderIndex);
		setCommandCSBufferView(info.modelIndex);
		m_cmdList->IASetVertexBuffers(0, 1, &m_vertexBufferView[meshIndex]);
		m_cmdList->IASetIndexBuffer(&m_indexBufferView[meshIndex]);
		m_cmdList->DrawIndexedInstanced(info.indicesNum,
			info.instanceNum, info.startPos, 0, 0);
	}
}

bool Renderer::drawMeshTranslucent(const XMFLOAT3& pos,
	int shaderIndex, int modelIndex, MeshData meshData,
	int indicesNum, int startPos, int instanceNum)
{
	if (m_meshTranslucentNum >= MaxMeshNum) return false;

	m_meshDrawTranslucentList[m_meshTranslucentNum] =
		MeshDrawInfo(modelIndex, shaderIndex,
			meshData, indicesNum, startPos, instanceNum);

	XMVECTOR p = XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	XMVECTOR q = XMVector4Transform(p, m_cameraViewMatrix);
	float z = q.m128_f32[2];
	m_meshTranslucentIndex[m_meshTranslucentNum] = { z, m_meshTranslucentNum };
	m_meshTranslucentNum += 1;

	return true;
}

int Renderer::allocatePointLight(const PointLight& light)
{
	if (m_pointLightNum >= MaxPointLightNum) return -1;

	int index = 0;
	for (; index < MaxPointLightNum; ++index)
	{
		if (m_availablePointLight[index]) break;
	}
	if (index >= MaxPointLightNum) return -1;

	m_pointLight[index] = light;
	m_availablePointLight[index] = false;
	m_pointLightNum += 1;
	return index;
}

void Renderer::setPointLihgt(int index, const PointLight& light)
{
	if (index < 0 || index >= MaxPointLightNum) return;
	if (m_availablePointLight[index] == true) return;

	m_pointLight[index] = light;
}

void Renderer::resetPointLightAll()
{
	PointLight light(ZeroVec3d, ZeroVec3d, 1.0f);
	for (int i = 0; i < MaxPointLightNum; ++i)
	{
		m_pointLight[i] = light;
		m_availablePointLight[i] = true;
	}
	m_pointLightNum = 0;
}

void Renderer::resetPointLight(int index)
{
	if (index < 0 || index >= MaxPointLightNum) return;
	if (m_availablePointLight[index] == true) return;

	m_pointLight[index] = PointLight(ZeroVec3d, ZeroVec3d, 1.0f);
	m_availablePointLight[index] = true;
	m_pointLightNum -= 1;
}

int Renderer::allocateSpotLight(const SpotLight& light)
{
	if (m_spotLightNum >= MaxSpotLightNum) return -1;

	int index = 0;
	for (; index < MaxSpotLightNum; ++index)
	{
		if (m_availableSpotLight[index]) break;
	}
	if (index >= MaxSpotLightNum) return -1;

	m_spotLight[index] = light;
	m_availableSpotLight[index] = false;
	m_spotLightNum += 1;
	return index;
}

void Renderer::setSpotLihgt(int index, const SpotLight& light)
{
	if (index < 0 || index >= MaxSpotLightNum) return;
	if (m_availableSpotLight[index] == true) return;

	m_spotLight[index] = light;
}

void Renderer::resetSpotLightAll()
{
	SpotLight light(ZeroVec3d, 1.0f, UnitVecX3d, 30.0f, ZeroVec3d);
	for (int i = 0; i < MaxSpotLightNum; ++i)
	{
		m_spotLight[i] = light;
		m_availableSpotLight[i] = true;
	}
	m_spotLightNum = 0;
}

void Renderer::resetSpotLight(int index)
{
	if (index < 0 || index >= MaxSpotLightNum) return;
	if (m_availableSpotLight[index] == true) return;

	m_spotLight[index] = SpotLight(ZeroVec3d, 1.0f, UnitVecX3d, 30.0f, ZeroVec3d);
	m_availableSpotLight[index] = true;
	m_spotLightNum -= 1;
}

XMFLOAT3 Renderer::getDepthVector()
{
	return XMFLOAT3(m_cameraViewMatrix.r[0].m128_f32[2],
					m_cameraViewMatrix.r[1].m128_f32[2],
					m_cameraViewMatrix.r[2].m128_f32[2]);
}

MeshData Renderer::createGridMesh(const wchar_t* meshName, const GridHeightMap& map)
{
	auto it = m_meshData.find(meshName);
	if (it != m_meshData.end()) return MeshData(-1, 0, 0);

	int index = 0;
	for (; index < MaxMeshDataNum; ++index)
	{
		if (m_vertexBuffer[index].Get() == nullptr) break;
	}
	if (index >= MaxMeshDataNum) return MeshData(-1, 0, 0);

	size_t vnum = (size_t)(map.xNum * map.yNum);
	float uStep = 1.0f / (float)(map.xNum - 1);
	float vStep = 1.0f / (float)(map.yNum - 1);

	// 頂点設定
	std::vector<VertexNorm> vertices(vnum);
	for (int y = 0; y < map.yNum; ++y)
	{
		for (int x = 0; x < map.xNum; ++x)
		{
			int index = x + y * map.xNum;
			vertices[index] = { map.heightMap[index],
				XMFLOAT2(uStep * x, vStep * y), UnitVecZ3d };
		}
	}

	// 法線計算
	for (int y = 0; y < map.yNum; ++y)
	{
		for (int x = 0; x < map.xNum; ++x)
		{
			int index = x + y * map.xNum;
			if (x == map.xNum - 1 && y == 0)
			{
				vertices[index].norm = calcNormVector(
					vertices[index + map.xNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
			}
			else if (x == 0 && y == map.yNum - 1)
			{
				vertices[index].norm = calcNormVector(
					vertices[index - map.xNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
			}
			else if (x == 0 && y == 0)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xNum + 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index + map.xNum + 1].pos - vertices[index].pos,
					vertices[index + map.xNum].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2);
			}
			else if (x == map.xNum - 1 && y == map.yNum - 1)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xNum - 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index - map.xNum - 1].pos - vertices[index].pos,
					vertices[index - map.xNum].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2);
			}
			else if (y == 0)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index + map.xNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index + map.xNum + 1].pos - vertices[index].pos,
					vertices[index + map.xNum].pos - vertices[index].pos);
				XMFLOAT3 n3 = calcNormVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xNum + 1].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2 + n3);
			}
			else if (y == map.yNum - 1)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xNum - 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index - map.xNum - 1].pos - vertices[index].pos,
					vertices[index - map.xNum].pos - vertices[index].pos);
				XMFLOAT3 n3 = calcNormVector(
					vertices[index - map.xNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2 + n3);
			}
			else if (x == 0)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index - map.xNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xNum + 1].pos - vertices[index].pos);
				XMFLOAT3 n3 = calcNormVector(
					vertices[index + map.xNum + 1].pos - vertices[index].pos,
					vertices[index + map.xNum].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2 + n3);
			}
			else if (x == map.xNum - 1)
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index - map.xNum - 1].pos - vertices[index].pos,
					vertices[index - map.xNum].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xNum - 1].pos - vertices[index].pos);
				XMFLOAT3 n3 = calcNormVector(
					vertices[index + map.xNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2 + n3);
			}
			else
			{
				XMFLOAT3 n1 = calcNormVector(
					vertices[index - 1].pos - vertices[index].pos,
					vertices[index - map.xNum - 1].pos - vertices[index].pos);
				XMFLOAT3 n2 = calcNormVector(
					vertices[index - map.xNum - 1].pos - vertices[index].pos,
					vertices[index - map.xNum].pos - vertices[index].pos);
				XMFLOAT3 n3 = calcNormVector(
					vertices[index - map.xNum].pos - vertices[index].pos,
					vertices[index + 1].pos - vertices[index].pos);
				XMFLOAT3 n4 = calcNormVector(
					vertices[index + 1].pos - vertices[index].pos,
					vertices[index + map.xNum + 1].pos - vertices[index].pos);
				XMFLOAT3 n5 = calcNormVector(
					vertices[index + map.xNum + 1].pos - vertices[index].pos,
					vertices[index + map.xNum].pos - vertices[index].pos);
				XMFLOAT3 n6 = calcNormVector(
					vertices[index + map.xNum].pos - vertices[index].pos,
					vertices[index - 1].pos - vertices[index].pos);
				vertices[index].norm = normalize(n1 + n2 + n3 + n4 + n5 + n6);
			}
		}
	}
	if (!createResourceBuffer(m_vertexBuffer[index].GetAddressOf(),
		vertices.size() * sizeof(VertexNorm))) return MeshData(-1, 0, 0);
	if (!uploadResourceBuffer(m_vertexBuffer[index].Get(),
		(void*)vertices.data(), vertices.size() * sizeof(VertexNorm)))
	{
		m_vertexBuffer[index].Reset();
		return MeshData(-1, 0, 0);
	}
	setVertexBufferView(m_vertexBufferView[index],
		m_vertexBuffer[index].Get(),
		(UINT)vertices.size() * sizeof(VertexNorm), sizeof(VertexNorm));

	int indicesNum = 6 * (map.xNum - 1) * (map.yNum - 1);
	std::vector<unsigned short> indices(indicesNum);
	int testIndex = 0;
	for (int ypoly = 0; ypoly < map.yNum - 1; ++ypoly)
	{
		for (int xpoly = 0; xpoly < map.xNum - 1; ++xpoly)
		{
			int v = xpoly + ypoly * map.xNum;
			int idx = 6 * (xpoly + ypoly * (map.xNum - 1));
			indices[idx] = v;
			indices[idx + 1] = v + map.xNum + 1;
			indices[idx + 2] = v + map.xNum;
			indices[idx + 3] = v;
			indices[idx + 4] = v + 1;
			indices[idx + 5] = v + map.xNum + 1;
		}
	}
	if (!createResourceBuffer(m_indexBuffer[index].GetAddressOf(),
		indices.size() * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		return MeshData(-1, 0, 0);
	}
	if (!uploadResourceBuffer(m_indexBuffer[index].Get(),
		(void*)indices.data(), indices.size() * sizeof(unsigned short)))
	{
		m_vertexBuffer[index].Reset();
		m_indexBuffer[index].Reset();
		return MeshData(-1, 0, 0);
	}
	setIndexBufferView(m_indexBufferView[index],
		m_indexBuffer[index].Get(), (UINT)indices.size() * sizeof(unsigned short));

	std::vector<BufferData> data(1);
	data[0].mesh = MeshData(index, (int)vertices.size(), (int)indices.size());
	m_meshData[meshName] = data;

	return data[0].mesh;
}

Ray Renderer::getCameraRayOnScreenPos(XMFLOAT2 spos)
{
	float halfWidth = 0.5f * m_game->getWidth();
	float halfHeight = 0.5f * m_game->getHeight();

	XMFLOAT3 devPos;
	devPos.x = (spos.x / halfWidth) - 1.0f;
	devPos.y = 1.0f - (spos.y / halfHeight);
	devPos.z = 0.0f;

	XMFLOAT3 scPos = mul(devPos, m_cameraInverseMatrix, 1.0f, true);
	XMFLOAT3 camPos = m_camera.getExtParam().eye;

	return Ray(camPos, normalize(scPos - camPos));
}

XMFLOAT2 Renderer::projectToScreenPos(XMFLOAT3 wpos)
{
	float halfWidth = 0.5f * m_game->getWidth();
	float halfHeight = 0.5f * m_game->getHeight();
	XMFLOAT3 devPos = mul(wpos, m_cameraMatrix, 1.0f, true);

	return XMFLOAT2(halfWidth * (devPos.x + 1.0f), halfHeight * (1.0f - devPos.y));
}

bool Renderer::createRTVDescHeap(ID3D12DescriptorHeap** dHeap, UINT dNum)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;
	desc.NumDescriptors = dNum;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;

	HRESULT hr = m_device->CreateDescriptorHeap(
		&desc, IID_PPV_ARGS(dHeap));

	return SUCCEEDED(hr);
}

void Renderer::setRenderTargetView(ID3D12Resource* buffer, DXGI_FORMAT format,
	ID3D12DescriptorHeap* dHeap, UINT index)
{
	auto handle = dHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += m_rtvIncSize * index;

	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format               = format;
	desc.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice   = 0;
	desc.Texture2D.PlaneSlice = 0;

	m_device->CreateRenderTargetView(buffer, &desc, handle);
}

bool Renderer::createOffscreen()
{
	D3D12_CLEAR_VALUE clear = { m_renderTargetFormat,
		{ m_backColor.x, m_backColor.y, m_backColor.z, 1.0f }};
	if (!createShaderResource(m_offscreenBuffer.GetAddressOf(),
		(UINT)m_game->getWidth(), (UINT)m_game->getHeight(),
		m_renderTargetFormat, 1, 1, D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		&clear)) return false;

	if (!createRTVDescHeap(m_offscreenRTVHeap.GetAddressOf(), 1)) return false;
	setRenderTargetView(m_offscreenBuffer.Get(), m_renderTargetFormat,
		m_offscreenRTVHeap.Get(), 0);

	if (!createDescHeap(m_offscreenSRVHeap.GetAddressOf(), 1)) return false;
	setShaderResourceView(m_offscreenBuffer.Get(), m_renderTargetFormat,
		m_offscreenSRVHeap.Get(), 0);

	VertexUV vertices[4];
	vertices[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } };
	vertices[1] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f } };
	vertices[2] = { {  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } };
	vertices[3] = { {  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f } };
	if (!createResourceBuffer(m_offscreenVertexBuffer.GetAddressOf(),
		4 * sizeof(VertexUV))) return false;
	if (!uploadResourceBuffer(m_offscreenVertexBuffer.Get(),
		(void*)vertices, 4 * sizeof(VertexUV))) return false;
	setVertexBufferView(m_offscreenVertexBufferView, m_offscreenVertexBuffer.Get(),
		4 * sizeof(VertexUV), sizeof(VertexUV));

	return true;
}

void Renderer::setPostEffectShader(int shaderIndex)
{
	if (m_postEffectIndex == shaderIndex) return;
	if (shaderIndex < 0 || shaderIndex >= PEShaderNum) return;

	m_postEffectIndex = shaderIndex;
}

