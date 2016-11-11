#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "Common\DDSTextureLoader.h"


namespace DX11UWA
{
	// This sample renderer instantiates a basic rendering pipeline.
	class My3DSceneRenderer
	{
	public:
		My3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, string pathV, string pathP, const char *path, string path2, string path3, string path4, int n, bool loadmodel);
		void CreateDeviceDependentResources(string pathV, string pathP);//, const char *path, string path2, string path3, string path4, int n, bool loadmodel
		void CreateWindowSizeDependentResources(void);
		void ReleaseDeviceDependentResources(void);
		void Update(DX::StepTimer const& timer);
		void Render(void);
		void StartTracking(void);
		void TrackingUpdate(float positionX);
		void StopTracking(void);
		inline bool IsTracking(void) { return m_tracking; }

		// Helper functions for keyboard and mouse input
		void SetKeyboardButtons(const char* list);
		void SetMousePosition(const Windows::UI::Input::PointerPoint^ pos);
		void SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos);

		//My function:
		void CreateCube(string path2, string path3,int n);
		void CreateGround(string path2, string path3,int n);
		void CreateModel(const char *path, string path2, string path3, string path4, int n);
		void ScaleModel(float x, float y, float z,int index);
		void TransModel(float x, float y, float z, int index);
		void TranlateModel(float sx, float sy, float sz, float tx, float ty, float tz, DX::StepTimer const& timer, float degree, int r, int index);
		void Calculatenormal(VertexPositionUVNormal V1, VertexPositionUVNormal V2, VertexPositionUVNormal V3);
		void CreateDirectionalLight();
		void CreatePointLight();
		void CreateSpotLight();
		void UpdataLight(DX::StepTimer const& timer, float const moveSpd, float const rotSpd);
		//void LoadMesh(const char *path);

	private:
		void Rotate(float radians, int index);
		void UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// Direct3D resources for cat geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catdiff;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catnorm;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_catspec;

		Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightd;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightp;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_lights;

		// System resources for cat geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		//light
		DirectionalLightConstantBuffer m_dcfd;
		PointLightConstantBuffer m_pcfd;
		SpotLightConstantBuffer m_scfd;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		// Data members for keyboard and mouse input
		char	m_kbuttons[256];
		Windows::UI::Input::PointerPoint^ m_currMousePos;
		Windows::UI::Input::PointerPoint^ m_prevMousePos;

		// Matrix data member for the camera
		DirectX::XMFLOAT4X4 m_camera;
		int num;


		const char *path;
		string path2;
		string path3;
		string path4;
		int n;
		bool loadmodel;
	};
}

