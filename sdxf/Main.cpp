#include "Window.h"
#include "DXWrapper.h"
#include "Scene.h"

static const float CAMERA_SPEED = 3.0f;
static const float RENDER_DISTANCE = 100.0f;

DXWrapper *g_DxWrapper;
Scene *g_Scene;

void PrepareScene() {
	Cube ground = Cube(XMFLOAT3(0.0f, -3.0f, 0.0f), XMFLOAT3(50.0f, 0.2f, 1000.0f));
	Cube street = Cube(XMFLOAT3(-2.0f, -2.9f, 0.0f), XMFLOAT3(0.5f, 0.2f, 1000.0f));
	Cube building = Cube(XMFLOAT3(0, 0, 0));
	g_Scene->AddObject(ground);
	g_Scene->AddObject(street);
	g_Scene->AddObject(building);
}

void Update(float deltaTime) {
	// update camera
	XMVECTOR eyePosition = XMVectorSet(0, 1.5, -10, 1);
	XMVECTOR focusPoint = XMVectorSet(0, 1.5, 0, 1);
	XMVECTOR upDirection = XMVectorSet(0, 1.5, 0, 0);
	XMMATRIX viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	static float cameraZ = 0.0f;
	cameraZ += CAMERA_SPEED * deltaTime;
	XMMATRIX camTranslationMatrix = XMMatrixTranslation(1.0f, 1.0f, -cameraZ);
	viewMatrix *= camTranslationMatrix;
	g_Scene->SetCameraPosition(viewMatrix);

	// update scene objects
	if (cameraZ > g_Scene->GetLastObject().location.z - RENDER_DISTANCE) {
		float height1 = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 - 0.5f)));
		float height2 = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (5 - 0.5f)));
		float lastObjectZ = g_Scene->GetLastObject().location.z;
		g_Scene->AddObject(Cube(XMFLOAT3(0, 0, lastObjectZ + 5), XMFLOAT3(1, height1, 1)));
		g_Scene->AddObject(Cube(XMFLOAT3(-2, 0, lastObjectZ + 5), XMFLOAT3(1, height2, 1)));
	}
}

void Run()
{
	MSG msg = { 0 };

	static DWORD previousTime = timeGetTime();
	static const float targetFramerate = 30.0f;
	static const float maxTimeStep = 1.0f / targetFramerate;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime) / 1000.0f;
			previousTime = currentTime;

			Update(deltaTime);
			g_DxWrapper->Render(g_Scene, deltaTime);
		}
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	Window window(hInstance);
	g_DxWrapper = new DXWrapper(window.handle);
	g_Scene = new Scene();

	PrepareScene();
	Run();

	delete g_DxWrapper;
	delete g_Scene;

	return 0;
}