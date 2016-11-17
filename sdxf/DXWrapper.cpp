#include "DXWrapper.h"

DXWrapper::DXWrapper(HWND & windowHandle)
{
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);

	// Compute the client dimensions
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceCtx);

	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating directx device and swap chain", L"Error", MB_OK | MB_ICONERROR);
	}
	// Initialize the back buffer
	ID3D11Texture2D* backBuffer;
	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed getting swap chain buffer", L"Error", MB_OK | MB_ICONERROR);
	}

	hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating render target view", L"Error", MB_OK | MB_ICONERROR);
	}

	backBuffer->Release();
	backBuffer = NULL;

	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = device->CreateTexture2D(&depthStencilBufferDesc, nullptr, &depthStencilBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating texture", L"Error", MB_OK | MB_ICONERROR);
	}

	hr = device->CreateDepthStencilView(depthStencilBuffer, nullptr, &depthStencilView);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating stencil view", L"Error", MB_OK | MB_ICONERROR);
	}
	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	hr = device->CreateDepthStencilState(&depthStencilStateDesc, &depthStencilState);

	// Setup rasterizer state.
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state object.
	hr = device->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating rasterizer state", L"Error", MB_OK | MB_ICONERROR);
	}

	viewport = { 0 };
	viewport.Width = static_cast<float>(clientWidth);
	viewport.Height = static_cast<float>(clientHeight);
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	deviceCtx->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	deviceCtx->OMSetDepthStencilState(depthStencilState, 1);

	deviceCtx->ClearRenderTargetView(renderTargetView, Colors::Black);
	deviceCtx->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	LoadCubeData(windowHandle);
}

void DXWrapper::Render(Scene *scene, float deltaTime)
{
	deviceCtx->ClearRenderTargetView(renderTargetView, Colors::Black);
	deviceCtx->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// camera
	deviceCtx->UpdateSubresource(g_d3dConstantBuffers[CB_Frame], 0, nullptr, &scene->GetCameraPosition(), 0, 0);
	
	// scene objects
	for (std::vector<int>::size_type i = 0; i != scene->GetSize(); i++) {
		Cube cube = scene->GetObjectAt(i);
		deviceCtx->UpdateSubresource(g_d3dConstantBuffers[CB_Object], 0, nullptr, &cube.GetTransform(), 0, 0);
		deviceCtx->VSSetConstantBuffers(0, 3, g_d3dConstantBuffers);
		deviceCtx->DrawIndexed(_countof(cube.indices), 0, 0);
	}

	swapChain->Present(0, 0);
}

void DXWrapper::LoadCubeData(HWND &windowHandle)
{
	LoadShaders();
	Cube cube = Cube(XMFLOAT3(0, 0, 0));
	// Create and initialize the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(cube.vertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = cube.vertices;

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &resourceData, &vertexBuffer);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Failed creating buffer", L"Error", MB_OK | MB_ICONERROR);
	}
	// Create and initialize the index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(cube.indices);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceData.pSysMem = cube.indices;

	hr = device->CreateBuffer(&indexBufferDesc, &resourceData, &indexBuffer);

	// Create the constant buffers for the variables defined in the vertex shader.
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Appliation]);
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Frame]);
	hr = device->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Object]);

	// Create the input layout for the vertex shader.
	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,Color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// Load the compiled vertex shader
	device->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &inputLayout);

	// Load the compiled pixel shader.
	device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &pixelShader);

	// Setup the projection matrix.
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);

	// Compute the exact client dimensions.
	// This is required for a correct projection matrix.
	float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

	deviceCtx->UpdateSubresource(g_d3dConstantBuffers[CB_Appliation], 0, nullptr, &projectionMatrix, 0, 0);

	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;
	deviceCtx->IASetVertexBuffers(0, 1, &vertexBuffer, &vertexStride, &offset);
	deviceCtx->IASetInputLayout(inputLayout);
	deviceCtx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceCtx->VSSetShader(vertexShader, nullptr, 0);
	deviceCtx->RSSetViewports(1, &viewport);
	deviceCtx->PSSetShader(pixelShader, nullptr, 0);
}

HRESULT DXWrapper::CompileShader(_In_ LPCWSTR srcFile, _In_ LPCSTR entryPoint, _In_ LPCSTR profile, _Outptr_ ID3DBlob** blob)
{
	if (!srcFile || !entryPoint || !profile || !blob)
		return E_INVALIDARG;

	*blob = nullptr;

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
	flags |= D3DCOMPILE_DEBUG;
#endif

	const D3D_SHADER_MACRO defines[] =
	{
		"EXAMPLE_DEFINE", "1",
		NULL, NULL
	};

	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(srcFile, defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint, profile,
		flags, 0, &shaderBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}

		if (shaderBlob)
			shaderBlob->Release();

		return hr;
	}

	*blob = shaderBlob;

	return hr;
}

void DXWrapper::LoadShaders() {
	CompileShader(L"SimpleVertexShader.hlsl", "SimpleVertexShader", "vs_4_0_level_9_3", &vertexShaderBlob);
	CompileShader(L"SimplePixelShader.hlsl", "SimplePixelShader", "ps_4_0_level_9_3", &pixelShaderBlob);

	device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &vertexShader);
	device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &pixelShader);
}