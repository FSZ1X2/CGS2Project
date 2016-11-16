#include "pch.h"
#include "DX11UWAMain.h"
#include "Common\DirectXHelper.h"

using namespace DX11UWA;
using namespace Windows::Foundation;
using namespace Windows::System::Threading;
using namespace Concurrency;

// Loads and initializes application assets when the application is loaded.
DX11UWAMain::DX11UWAMain(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources)
{
	// Register to be notified if the Device is lost or recreated
	m_deviceResources->RegisterDeviceNotify(this);

	// TODO: Replace this with your app's content initialization.
	//m_sceneRenderer = std::unique_ptr<Sample3DSceneRenderer>(new Sample3DSceneRenderer(m_deviceResources));
	//
	//m_fpsTextRenderer = std::unique_ptr<SampleFpsTextRenderer>(new SampleFpsTextRenderer(m_deviceResources));
	//m_sceneRenderer = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "VertexShaderMulti.cso", "PixelShaderNorm.cso"));
	//m_sceneRendererCube = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "VertexShaderMulti.cso", "PixelShaderNorm.cso"));
	//m_sceneRendererGround = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "SampleVertexShader.cso", "PixelShaderMultitexture.cso"));
	//m_sceneRendererTree = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "SampleVertexShader.cso", "SamplePixelShader.cso"));
	m_Skycube = std::unique_ptr<SkyBox>(new SkyBox(m_deviceResources));

	m_sceneRenderer = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "VertexShaderMulti.cso", "PixelShaderNorm.cso","Assets/cat.obj", "Assets/cat_diff.dds", "Assets/cat_norm.dds", "Assets/cat_spec.dds", 1, 0));
	m_sceneRendererCube = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "VertexShaderMulti.cso", "PixelShaderNorm.cso","Assets/Tent_1.obj", "Assets/tent_exterior_d.dds", "Assets/tent_exterior_n.dds", "", 1, 0));
	m_sceneRendererGround = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "SampleVertexShader.cso", "PixelShaderMultitexture.cso", "","Assets/grass_seamless.dds", "Assets/Box_Circuit.dds", "", 1, 1));
	m_sceneRendererTree = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "SampleVertexShader.cso", "SamplePixelShader.cso", "Assets/palmTree.obj", "Assets/palm1.dds", "", "", 4,0));

	m_sceneRendererTC = std::unique_ptr<My3DSceneRenderer>(new My3DSceneRenderer(m_deviceResources, "SampleVertexShader.cso", "CubePixel.cso", "Assets/palmTree.obj", "Assets/cube.dds", "", "", 3, 2));

	m_fpsTextRenderer = std::unique_ptr<MyFpsTextRenderer>(new MyFpsTextRenderer(m_deviceResources));

	// TODO: Change the timer settings if you want something other than the default variable timestep mode.
	// e.g. for 60 FPS fixed timestep update logic, call:
	//m_sceneRenderer->CreateModel("Assets/cat.obj", "Assets/cat_diff.dds", "Assets/cat_norm.dds", "Assets/cat_spec.dds",1);
	//m_sceneRendererTC->CreateCube("Assets/cube.dds","",3);
	//m_sceneRendererCube->CreateModel("Assets/Tent_1.obj", "Assets/tent_exterior_d.dds", "Assets/tent_exterior_n.dds", "", 1);
	//m_sceneRendererGround->CreateGround("Assets/grass_seamless.dds", "Assets/Box_Circuit.dds",1);
	//m_timer.SetFixedTimeStep(true);
	//m_timer.SetTargetElapsedSeconds(1.0 / 60);
}

DX11UWAMain::~DX11UWAMain(void)
{
	// Deregister device notification
	m_deviceResources->RegisterDeviceNotify(nullptr);
}

// Updates application state when the window size changes (e.g. device orientation change)
void DX11UWAMain::CreateWindowSizeDependentResources(void)
{
	// TODO: Replace this with the size-dependent initialization of your app's content.
	m_sceneRenderer->CreateWindowSizeDependentResources();
	m_sceneRendererCube->CreateWindowSizeDependentResources();
	m_sceneRendererGround->CreateWindowSizeDependentResources();
	m_sceneRendererTree->CreateWindowSizeDependentResources();
	m_Skycube->CreateWindowSizeDependentResources();
	m_sceneRendererTC->CreateWindowSizeDependentResources();
}

// Updates the application state once per frame.
void DX11UWAMain::Update(void)
{
	// Update scene objects.
	m_timer.Tick([&]()
	{
		// TODO: Replace this with your app's content update functions.
		m_sceneRenderer->Update(m_timer);
		m_sceneRenderer->TranlateModel(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 3.0f, m_timer, 0, 0, 0);
		m_sceneRenderer->SetInputDeviceData(main_kbuttons, main_currentpos);
		m_sceneRendererCube->Update(m_timer);
		m_sceneRendererCube->TranlateModel(0.1f, 0.1f, 0.1f, 0.0f, 0.0f, 15.0f, m_timer, 0, 0, 0);
		m_sceneRendererCube->SetInputDeviceData(main_kbuttons, main_currentpos);
		m_sceneRendererGround->Update(m_timer);
		m_sceneRendererGround->TranlateModel(10.0f, 1.0f, 10.0f, 0.0f, 1.0f, 0.0f, m_timer, 0, 0, 0);
		m_sceneRendererGround->SetInputDeviceData(main_kbuttons, main_currentpos);
		m_sceneRendererTree->Update(m_timer);
		m_sceneRendererTree->TranlateModel(0.1f, 0.1f, 0.1f, 12.0f, 0.0f, 7.0f, m_timer, 0, 0,0);
		m_sceneRendererTree->TranlateModel(0.1f, 0.1f, 0.1f, -10.0f, 0.0f, 9.0f, m_timer, 0, 0,1);
		m_sceneRendererTree->TranlateModel(0.1f, 0.1f, 0.1f, 15.0f, 0.0f, 5.0f, m_timer, 0, 0,2);
		m_sceneRendererTree->TranlateModel(0.1f, 0.1f, 0.1f, 10.0f, 0.0f, 2.0f, m_timer, 0, 0,3);
		m_sceneRendererTree->SetInputDeviceData(main_kbuttons, main_currentpos);

		m_sceneRendererTC->Update(m_timer);
		m_sceneRendererTC->SwapCube(6.0f);
		m_sceneRendererTC->TranlateModel(1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 2.0f, m_timer, 0, 0, m_sceneRendererTC->cubeindex[0]);
		m_sceneRendererTC->TranlateModel(1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 4.0f, m_timer, 0, 0, m_sceneRendererTC->cubeindex[1]);
		m_sceneRendererTC->TranlateModel(1.0f, 1.0f, 1.0f, 2.0f, 1.0f, 6.0f, m_timer, 0, 0, m_sceneRendererTC->cubeindex[2]);
		m_sceneRendererTC->SetInputDeviceData(main_kbuttons, main_currentpos);

		m_Skycube->Update(m_timer);
		m_Skycube->SetInputDeviceData(main_kbuttons, main_currentpos);
		m_fpsTextRenderer->Update(m_timer);
	});
}

// Renders the current frame according to the current application state.
// Returns true if the frame was rendered and is ready to be displayed.
bool DX11UWAMain::Render(void)
{
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
	{
		return false;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Reset the viewport to target the whole screen.
	auto viewport = m_deviceResources->GetScreenViewport();
	context->RSSetViewports(1, &viewport);

	//auto viewport2 = m_deviceResources->GetScreenViewport();
	//context->RSSetViewports(2, &viewport2);

	// Reset render targets to the screen.
	ID3D11RenderTargetView *const targets[1] = { m_deviceResources->GetBackBufferRenderTargetView() };
	context->OMSetRenderTargets(1, targets, m_deviceResources->GetDepthStencilView());

	// Clear the back buffer and depth stencil view.
	context->ClearRenderTargetView(m_deviceResources->GetBackBufferRenderTargetView(), DirectX::Colors::CornflowerBlue);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_Skycube->Render();
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	// Render the scene objects.
	// TODO: Replace this with your app's content rendering functions.
	context->RSSetViewports(1, &viewport);
	m_sceneRenderer->Render();
	context->RSSetViewports(1, &viewport);
	m_sceneRendererTree->Render();
	context->RSSetViewports(1, &viewport);
	m_sceneRendererCube->Render();
	context->RSSetViewports(1, &viewport);
	m_sceneRendererGround->Render();
	context->RSSetViewports(1, &viewport);
	m_sceneRendererTC->Render();
	context->RSSetViewports(1, &viewport);
	m_fpsTextRenderer->Render();

	return true;
}

// Notifies renderers that device resources need to be released.
void DX11UWAMain::OnDeviceLost(void)
{
	m_sceneRenderer->ReleaseDeviceDependentResources();
	m_sceneRendererTree->ReleaseDeviceDependentResources();
	m_sceneRendererTC->ReleaseDeviceDependentResources();
	//m_sceneRendererCube->ReleaseDeviceDependentResources();
	m_sceneRendererGround->ReleaseDeviceDependentResources();
	m_Skycube->ReleaseDeviceDependentResources();
	m_fpsTextRenderer->ReleaseDeviceDependentResources();
}

// Notifies renderers that device resources may now be recreated.
void DX11UWAMain::OnDeviceRestored(void)
{
	m_sceneRenderer->CreateDeviceDependentResources("VertexShaderMulti.cso", "PixelShaderNorm.cso");//, "Assets/cat.obj", "Assets/cat_diff.dds", "Assets/cat_norm.dds", "Assets/cat_spec.dds", 1, true
	m_sceneRendererCube->CreateDeviceDependentResources("VertexShaderMulti.cso", "PixelShaderNorm.cso");//, "Assets/Tent_1.obj", "Assets/tent_exterior_d.dds", "Assets/tent_exterior_n.dds", "", 1, true
	m_sceneRendererGround->CreateDeviceDependentResources("SampleVertexShader.cso", "PixelShaderMultitexture.cso");//, "", "Assets/grass_seamless.dds", "Assets/Box_Circuit.dds", "", 1, false
	m_sceneRendererTree->CreateDeviceDependentResources("SampleVertexShader.cso", "SamplePixelShader.cso");//, "Assets/palmTree.obj", "Assets/palm1.dds", "", "", 4, true
	m_sceneRendererTC->CreateDeviceDependentResources("SampleVertexShader.cso", "SamplePixelShader.cso");//, "Assets/palmTree.obj", "Assets/palm1.dds", "", "", 4, true
	//m_sceneRenderer->CreateDeviceDependentResources("VertexShaderMulti.cso", "PixelShaderNorm.cso");
	//m_sceneRendererTree->CreateDeviceDependentResources("SampleVertexShader.cso", "SamplePixelShader.cso");
	//m_sceneRendererCube->CreateDeviceDependentResources("VertexShaderMulti.cso", "PixelShaderNorm.cso");
	//m_sceneRendererGround->CreateDeviceDependentResources("SampleVertexShader.cso", "PixelShaderMultitexture.cso");
	m_Skycube->CreateDeviceDependentResources();
	m_fpsTextRenderer->CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

void DX11UWAMain::GetKeyboardButtons(const char* buttons)
{
	memcpy_s(main_kbuttons, sizeof(main_kbuttons), buttons, sizeof(main_kbuttons));
}

void DX11UWAMain::GetMousePos(const Windows::UI::Input::PointerPoint^ pos)
{
	main_currentpos = const_cast< Windows::UI::Input::PointerPoint^>(pos);
}
