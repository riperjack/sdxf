#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include "Scene.h"
#include "Cube.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "Winmm.lib")

using namespace DirectX;

class DXWrapper {
public:
	DXWrapper::DXWrapper(HWND & windowHandle);
	void Render(Scene *scene, float deltaTime);
private:
	ID3D11Device* device;
	ID3D11DeviceContext* deviceCtx;
	IDXGISwapChain* swapChain;
	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11RasterizerState* rasterizerState;
	D3D11_VIEWPORT viewport;

	// Vertex buffer data
	ID3D11InputLayout* inputLayout;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	// Shader data
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3DBlob* vertexShaderBlob;
	ID3DBlob* pixelShaderBlob;

	enum ConstanBuffer
	{
		CB_Appliation,
		CB_Frame,
		CB_Object,
		NumConstantBuffers
	};
	ID3D11Buffer* g_d3dConstantBuffers[NumConstantBuffers];
	XMMATRIX projectionMatrix;

	void LoadCubeData(HWND &windowHandle);
	void LoadShaders();
	HRESULT CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob);
};


