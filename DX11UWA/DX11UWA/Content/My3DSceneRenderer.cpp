#include "pch.h"
#include "My3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace DX11UWA;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
My3DSceneRenderer::My3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, string pathV, string pathP) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	memset(m_kbuttons, 0, sizeof(m_kbuttons));
	m_currMousePos = nullptr;
	m_prevMousePos = nullptr;
	memset(&m_camera, 0, sizeof(XMFLOAT4X4));

	CreateDeviceDependentResources(pathV, pathP);
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void My3DSceneRenderer::CreateWindowSizeDependentResources(void)
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void My3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		//TransModel(0.0f, 0.0f, 0.0f);
		Rotate(radians);
	}


	// Update or move camera here
	UpdateCamera(timer, 1.0f, 0.75f);

}

// Rotate the 3D cube model a set amount of radians.
void My3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void My3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd)
{
	const float delta_time = (float)timer.GetElapsedSeconds();

	if (m_kbuttons['W'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['S'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['A'])
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['D'])
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['X'])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, -moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons[VK_SPACE])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}

	if (m_currMousePos) 
	{
		if (m_currMousePos->Properties->IsRightButtonPressed && m_prevMousePos)
		{
			float dx = m_currMousePos->Position.X - m_prevMousePos->Position.X;
			float dy = m_currMousePos->Position.Y - m_prevMousePos->Position.Y;

			XMFLOAT4 pos = XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);

			m_camera._41 = 0;
			m_camera._42 = 0;
			m_camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotSpd * delta_time);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotSpd * delta_time);

			XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&m_camera, temp_camera);

			m_camera._41 = pos.x;
			m_camera._42 = pos.y;
			m_camera._43 = pos.z;
		}
		m_prevMousePos = m_currMousePos;
	}


}

void My3DSceneRenderer::SetKeyboardButtons(const char* list)
{
	memcpy_s(m_kbuttons, sizeof(m_kbuttons), list, sizeof(m_kbuttons));
}

void My3DSceneRenderer::SetMousePosition(const Windows::UI::Input::PointerPoint^ pos)
{
	m_currMousePos = const_cast<Windows::UI::Input::PointerPoint^>(pos);
}

void My3DSceneRenderer::SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos)
{
	SetKeyboardButtons(kb);
	SetMousePosition(pos);
}

void DX11UWA::My3DSceneRenderer::StartTracking(void)
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void My3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void My3DSceneRenderer::StopTracking(void)
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void My3DSceneRenderer::Render(void)
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	//set texture
	context->PSSetShaderResources(0, 1, m_catdiff.GetAddressOf());
	context->PSSetShaderResources(1, 1, m_catnorm.GetAddressOf());
	context->PSSetShaderResources(2, 1, m_catspec.GetAddressOf());
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
}

void My3DSceneRenderer::CreateDeviceDependentResources(string pathV, string pathP)
{
	std::wstring V = std::wstring(pathV.begin(), pathV.end());
	auto loadVSTask = DX::ReadDataAsync(V);
	std::wstring P = std::wstring(pathP.begin(), pathP.end());
	auto loadPSTask = DX::ReadDataAsync(P);
	// Load shaders asynchronously.
	//auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	//auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_inputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]()
	{

	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]()
	{
		m_loadingComplete = true;
	});
}

void My3DSceneRenderer::ReleaseDeviceDependentResources(void)
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
}

//My function:
void My3DSceneRenderer::CreateModel(const char *path, string path2, string path3, string path4)
{
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector< XMFLOAT3 > temp_vertices;
	std::vector< XMFLOAT3 > temp_uvs;
	std::vector< XMFLOAT3 > temp_normals;

	std::vector< XMFLOAT3 > out_vertices;
	std::vector< XMFLOAT3 > out_uvs;
	std::vector< XMFLOAT3 > out_normals;

	FILE * file;
	if (fopen_s(&file, path, "r") != 0)
	{
		printf("Impossible to open the file !\n");
	}
	else
	{
		while (1)
		{
			char lineHeader[128];
			// read the first word of the line
			int res = fscanf_s(file, "%s", lineHeader, 128);
			if (res == EOF)
				break;
			if (strcmp(lineHeader, "v") == 0)
			{
				XMFLOAT3 vertex;
				fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
				temp_vertices.push_back(vertex);
			}
			else if (strcmp(lineHeader, "vt") == 0)
			{
				XMFLOAT3 uv;
				fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
				uv.y = 1.0f - uv.y;
				temp_uvs.push_back(uv);
			}
			else if (strcmp(lineHeader, "vn") == 0)
			{
				XMFLOAT3 normal;
				fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
				temp_normals.push_back(normal);
			}
			else if (strcmp(lineHeader, "f") == 0)
			{
				std::string vertex1, vertex2, vertex3;
				unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2]);
				if (matches != 9)
				{
					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					break;
				}
				vertexIndices.push_back(vertexIndex[0]);
				vertexIndices.push_back(vertexIndex[1]);
				vertexIndices.push_back(vertexIndex[2]);
				uvIndices.push_back(uvIndex[0]);
				uvIndices.push_back(uvIndex[1]);
				uvIndices.push_back(uvIndex[2]);
				normalIndices.push_back(normalIndex[0]);
				normalIndices.push_back(normalIndex[1]);
				normalIndices.push_back(normalIndex[2]);
			}
		}
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i++)
	{
		unsigned int vertexIndex = vertexIndices[i];
		XMFLOAT3 vertex = temp_vertices[vertexIndex - 1];
		out_vertices.push_back(vertex);

		unsigned int uvIndex = uvIndices[i];
		XMFLOAT3 uvs = temp_uvs[uvIndex - 1];
		out_uvs.push_back(uvs);

		unsigned int normalIndex = normalIndices[i];
		XMFLOAT3 normal = temp_normals[normalIndex - 1];
		out_normals.push_back(normal);
	}
	

	size_t sizeofvertex = out_vertices.size();
	std::vector< VertexPositionUVNormal > LoadModels;
	for (size_t i = 0; i < sizeofvertex; i++)
	{
		VertexPositionUVNormal LoadModel;
		LoadModel.pos.x = out_vertices[i].x;
		LoadModel.pos.y = out_vertices[i].y;
		LoadModel.pos.z = out_vertices[i].z;

		LoadModel.uv.x = out_uvs[i].x;
		LoadModel.uv.y = out_uvs[i].y;
		LoadModel.uv.z = 0.0f;// out_uvs[i].z;

		LoadModel.normal.x = out_normals[i].x;
		LoadModel.normal.y = out_normals[i].y;
		LoadModel.normal.z = out_normals[i].z;

		LoadModels.push_back(LoadModel);
	}

	for (unsigned int i = 0; i < vertexIndices.size(); i+=3)
	{
		Calculatenormal(LoadModels[i], LoadModels[i + 1], LoadModels[i + 2]);
	}

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &LoadModels[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(LoadModels.size() * sizeof(VertexPositionUVNormal), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

	size_t sizeofindex = vertexIndices.size();
	std::vector<unsigned int> LoadModelIndex;
	for (size_t i = 0; i < sizeofindex; i++)
	{
		LoadModelIndex.push_back(i);
	}

	m_indexCount = (unsigned int)sizeofindex;

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &LoadModelIndex[0];
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(LoadModelIndex.size()*sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));

	std::wstring temp = std::wstring(path2.begin(), path2.end());
	const wchar_t* p = temp.c_str();
	HRESULT hr = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), p, NULL, m_catdiff.GetAddressOf());
	DX::ThrowIfFailed(hr);
	if (path3 != "")
	{
		std::wstring temp2 = std::wstring(path3.begin(), path3.end());
		const wchar_t* p2 = temp2.c_str();
		HRESULT hr2 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), p2, NULL, m_catnorm.GetAddressOf());
		DX::ThrowIfFailed(hr2);
	}
	if (path4 != "")
	{
		std::wstring temp3 = std::wstring(path4.begin(), path4.end());
		const wchar_t* p3 = temp3.c_str();
		HRESULT hr3 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), p3, NULL, m_catspec.GetAddressOf());
		DX::ThrowIfFailed(hr3);
	}
}

/*void My3DSceneRenderer::LoadMesh(const char *path)
{
	// TODO: Load mesh data and send it to the graphics card.
	std::fstream file;

	file.open(path, std::ios_base::binary | std::ios_base::in);

	std::vector< VertexPositionUVNormal > vertexData;
	std::vector< unsigned int > indexData;
	string textureNames;

	if (file.is_open())
	{
		unsigned int len;
		file.read((char *)&len, sizeof(unsigned int));
		char *meshname;
		meshname = new char[len];
		file.read(meshname, len);

		unsigned int tcount;
		file.read((char *)&tcount, sizeof(unsigned int));
		char *savetextureNames;
		for (unsigned int i = 0; i < tcount; i++)
		{
			file.read((char *)&len, sizeof(len));
			savetextureNames = new char[len];
			file.read(savetextureNames, len);
			string save1 = savetextureNames;
			size_t found = save1.find_last_of("/\\");
			textureNames = save1.substr(found + 1);
			delete[] savetextureNames;
		}

		unsigned int vcount;
		file.read((char *)&vcount, sizeof(unsigned int));
		for (unsigned int i = 0; i < vcount; i++)
		{
			VertexPositionUVNormal data;
			file.read((char*)&data.pos.x, sizeof(float));
			file.read((char*)&data.pos.y, sizeof(float));
			file.read((char*)&data.pos.z, sizeof(float));

			file.read((char*)&data.normal.x, sizeof(float));
			file.read((char*)&data.normal.y, sizeof(float));
			file.read((char*)&data.normal.z, sizeof(float));

			file.read((char*)&data.uv.x, sizeof(float));
			file.read((char*)&data.uv.y, sizeof(float));

			data.pos.x *= 0.02f;
			data.pos.y *= 0.02f;
			data.pos.z *= 0.02f;

			data.uv.x = 1 - data.uv.x;
			data.uv.y = 1 - data.uv.y;
			data.uv.z = 0.0f;

			vertexData.push_back(data);
		}

		unsigned int icount;
		file.read((char *)&icount, sizeof(unsigned int));
		unsigned int *index;
		index = new unsigned int[icount * 3];
		file.read((char*)&index[0], 4 * (icount * 3));

		for (int i = 0; i < icount * 3; i++)
		{
			indexData.push_back(index[i]);
		}

		file.close();

		delete[] meshname;
		delete[] index;
	}
	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = &vertexData[0];
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(vertexData.size() * sizeof(VertexPositionUVNormal), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = &indexData[0];
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(indexData.size() * sizeof(unsigned int), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
}*/

void My3DSceneRenderer::CreateCube(string path2, string path3)
{
	static const VertexPositionUVNormal cubeVertices[] =
	{
		{ XMFLOAT3(-0.5f, -1.0f, -0.1f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-0.44f,-0.89f,-0.09f) },
		{ XMFLOAT3(-0.5f, -1.0f,  0.9f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(-0.35f,-0.7f,0.63f) },
		{ XMFLOAT3(-0.5f,  0.0f, -0.1f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(-0.9f,0.0f,-0.2f) },
		{ XMFLOAT3(-0.5f,  0.0f,  0.9f), XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT3(-0.49f,0.0f,0.87f) },
		{ XMFLOAT3(0.5f, -1.0f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f),  XMFLOAT3(0.44f,-0.89f,-0.09f) },
		{ XMFLOAT3(0.5f, -1.0f,  0.9f), XMFLOAT3(0.0f, 0.0f, 0.0f),  XMFLOAT3(0.35f,-0.7f,0.63f) },
		{ XMFLOAT3(0.5f,  0.0f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f),  XMFLOAT3(0.9f,0.0f,-0.2f) },
		{ XMFLOAT3(0.5f,  0.0f,  0.9f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(0.49f,0.0f,0.87f) },

		{ XMFLOAT3(-0.5f, -1.0f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(-0.44f,-0.89f,-0.09f) },
		{ XMFLOAT3(-0.5f, -1.0f,  0.9f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-0.35f,-0.7f,0.63f) },
		{ XMFLOAT3(0.5f, -1.0f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f),  XMFLOAT3(0.44f,-0.89f,-0.09f) },
		{ XMFLOAT3(0.5f, -1.0f,  0.9f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(0.35f,-0.7f,0.63f) },

		{ XMFLOAT3(-0.5f,  0.0f, -0.1f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(-0.9f,0.0f,-0.2f) },
		{ XMFLOAT3(-0.5f,  0.0f,  0.9f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(-0.49f,0.0f,0.87f) },
		{ XMFLOAT3(0.5f,  0.0f, -0.1f), XMFLOAT3(1.0f, 1.0f, 0.0f),  XMFLOAT3(0.9f,0.0f,-0.2f) },
		{ XMFLOAT3(0.5f,  0.0f,  0.9f), XMFLOAT3(1.0f, 0.0f, 0.0f),  XMFLOAT3(0.49f,0.0f,0.87f) },
	};

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = cubeVertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

	static const unsigned int cubeIndices[] =
	{
		0,1,2, // -x
		1,3,2,

		4,6,5, // +x
		5,6,7,

		0,6,4, // -z
		0,2,6,

		1,7,3, // +z
		1,5,7,

		8,9,11, //-y
		8,11,10,

		12,13,15, //+y
		12,15,14,
	};

	m_indexCount = ARRAYSIZE(cubeIndices);

	for (unsigned int i = 0; i < m_indexCount; i += 3)
	{
		Calculatenormal(cubeVertices[i], cubeVertices[i + 1], cubeVertices[i + 2]);
	}

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));

	std::wstring temp = std::wstring(path2.begin(), path2.end());
	const wchar_t* p = temp.c_str();
	HRESULT hr = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), p, NULL, m_catdiff.GetAddressOf());
	DX::ThrowIfFailed(hr);
	if (path3 != "")
	{
		std::wstring temp2 = std::wstring(path3.begin(), path3.end());
		const wchar_t* p2 = temp2.c_str();
		HRESULT hr2 = CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), p2, NULL, m_catnorm.GetAddressOf());
		DX::ThrowIfFailed(hr2);
	}
}

void My3DSceneRenderer::ScaleModel(float x, float y, float z)
{
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixScaling(x, y, z)));
}

void My3DSceneRenderer::TransModel(float x, float y, float z)
{
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(x, y, z)));
}

void My3DSceneRenderer::TranlateModel(float sx, float sy, float sz, float tx, float ty, float tz, DX::StepTimer const& timer, int r)
{
	if(r = 0)
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(tx, ty, tz)*XMMatrixScaling(sx, sy, sz)));
	else
	{
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));
		XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixTranslation(tx, ty, tz)*XMMatrixScaling(sx, sy, sz)*XMMatrixRotationY(radians)));
	}
}

void My3DSceneRenderer::Calculatenormal(VertexPositionUVNormal V1, VertexPositionUVNormal V2, VertexPositionUVNormal V3)
{
	VertexPositionUVNormal vert0 = V1;
	VertexPositionUVNormal vert1 = V2;
	VertexPositionUVNormal vert2 = V3;
	DirectX::XMFLOAT3 vertEdge0;
	vertEdge0.x = vert1.pos.x - vert0.pos.x;
	vertEdge0.y = vert1.pos.y - vert0.pos.y;
	vertEdge0.z = vert1.pos.z - vert0.pos.z;
	DirectX::XMFLOAT3 vertEdge1;
	vertEdge1.x = vert2.pos.x - vert0.pos.x;
	vertEdge1.y = vert2.pos.y - vert0.pos.y;
	vertEdge1.z = vert2.pos.z - vert0.pos.z;

	DirectX::XMFLOAT3 tex0 = vert0.uv;
	DirectX::XMFLOAT3 tex1 = vert1.uv;
	DirectX::XMFLOAT3 tex2 = vert2.uv;

	DirectX::XMFLOAT3 texEdge0;
	texEdge0.x = tex1.x - tex0.x;
	texEdge0.y = tex1.y - tex0.y;
	texEdge0.z = tex1.z - tex0.z;
	DirectX::XMFLOAT3 texEdge1;
	texEdge1.x = tex2.x - tex1.x;
	texEdge1.y = tex2.y - tex1.y;
	texEdge1.z = tex2.z - tex1.z;

	float ratio = 1.0f / (texEdge0.x * texEdge1.y - texEdge1.x * texEdge0.y);
	DirectX::XMFLOAT3 uDirection = DirectX::XMFLOAT3((texEdge1.y * vertEdge0.x - texEdge0.y * vertEdge1.x) * ratio, (texEdge1.y * vertEdge0.y - texEdge0.y * vertEdge1.y) * ratio, (texEdge1.y * vertEdge0.z - texEdge0.y * vertEdge1.z) * ratio);
	DirectX::XMFLOAT3 vDirection = DirectX::XMFLOAT3((texEdge0.x * vertEdge1.x - texEdge1.x * vertEdge0.x) * ratio, (texEdge0.x * vertEdge1.y - texEdge1.x * vertEdge0.y) * ratio, (texEdge0.x * vertEdge1.z - texEdge1.x * vertEdge0.z) * ratio);

	float lenuD = sqrt((uDirection.x * uDirection.x) + (uDirection.y * uDirection.y) + (uDirection.z * uDirection.z));
	uDirection.x = uDirection.x / lenuD;
	uDirection.y = uDirection.y / lenuD;
	uDirection.z = uDirection.z / lenuD;
	float dotResult1 = vert0.normal.x*uDirection.x + vert0.normal.y*uDirection.y + vert0.normal.z*uDirection.z;
	float dotResult2 = vert1.normal.x*uDirection.x + vert1.normal.y*uDirection.y + vert1.normal.z*uDirection.z;
	float dotResult3 = vert2.normal.x*uDirection.x + vert2.normal.y*uDirection.y + vert2.normal.z*uDirection.z;
	DirectX::XMFLOAT4 Tangent1, Tangent2, Tangent3;
	Tangent1.x = uDirection.x - vert0.normal.x * dotResult1;
	Tangent2.x = uDirection.x - vert1.normal.x * dotResult2;
	Tangent3.x = uDirection.x - vert2.normal.x * dotResult3;

	Tangent1.y = uDirection.y - vert0.normal.y * dotResult1;
	Tangent2.y = uDirection.y - vert1.normal.y * dotResult2;
	Tangent3.y = uDirection.y - vert2.normal.y * dotResult3;

	Tangent1.z = uDirection.z - vert0.normal.z * dotResult1;
	Tangent2.z = uDirection.z - vert1.normal.z * dotResult2;
	Tangent3.z = uDirection.z - vert2.normal.z * dotResult3;

	float lenT1 = sqrt((Tangent1.x * Tangent1.x) + (Tangent1.y * Tangent1.y) + (Tangent1.z * Tangent1.z));
	float lenT2 = sqrt((Tangent2.x * Tangent2.x) + (Tangent2.y * Tangent2.y) + (Tangent2.z * Tangent2.z));
	float lenT3 = sqrt((Tangent3.x * Tangent3.x) + (Tangent3.y * Tangent3.y) + (Tangent3.z * Tangent3.z));

	Tangent1.x = Tangent1.x / lenT1;
	Tangent1.y = Tangent1.y / lenT1;
	Tangent1.z = Tangent1.z / lenT1;

	Tangent2.x = Tangent2.x / lenT1;
	Tangent2.y = Tangent2.y / lenT1;
	Tangent2.z = Tangent2.z / lenT1;

	Tangent3.x = Tangent3.x / lenT1;
	Tangent3.y = Tangent3.y / lenT1;
	Tangent3.z = Tangent3.z / lenT1;

	float lenvD = sqrt((uDirection.x * uDirection.x) + (uDirection.y * uDirection.y) + (uDirection.z * uDirection.z));
	vDirection.x = vDirection.x / lenvD;
	vDirection.y = vDirection.y / lenvD;
	vDirection.z = vDirection.z / lenvD;

	DirectX::XMVECTOR cross1 = DirectX::XMVector3Cross(XMLoadFloat3(&vert0.normal), XMLoadFloat3(&uDirection));
	DirectX::XMVECTOR cross2 = DirectX::XMVector3Cross(XMLoadFloat3(&vert1.normal), XMLoadFloat3(&uDirection));
	DirectX::XMVECTOR cross3 = DirectX::XMVector3Cross(XMLoadFloat3(&vert2.normal), XMLoadFloat3(&uDirection));
	DirectX::XMFLOAT3 handedness = vDirection;
	float dotResultv1 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(cross1, XMLoadFloat3(&handedness)));
	float dotResultv2 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(cross2, XMLoadFloat3(&handedness)));
	float dotResultv3 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(cross3, XMLoadFloat3(&handedness)));

	Tangent1.w = (dotResultv1 < 0.0f) ? -1.0f : 1.0f;
	Tangent2.w = (dotResultv2 < 0.0f) ? -1.0f : 1.0f;
	Tangent3.w = (dotResultv3 < 0.0f) ? -1.0f : 1.0f;

	V1.tangent = Tangent1;
	V2.tangent = Tangent2;
	V3.tangent = Tangent3;
}
