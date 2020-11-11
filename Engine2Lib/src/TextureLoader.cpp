#include "pch.h"
#include "TextureLoader.h"
#include <wincodec.h>

namespace Engine2
{
	AssetStore<Texture> TextureLoader::Textures;
	std::string TextureLoader::LastResult;

	std::shared_ptr<Texture> TextureLoader::LoadTexture(const std::string& filename)
	{
		std::shared_ptr<Texture> t = Textures[filename];

		if (t) return t; // already loaded

		t = CreateTexture(filename);

		if (t) Textures.StoreAsset(filename, t);

		return t;
	}

	std::shared_ptr<Texture> TextureLoader::CreateTexture(const std::string& filename)
	{
		UINT width, height;

		Microsoft::WRL::ComPtr<IWICImagingFactory> pFactory = nullptr;

		HRESULT hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&pFactory)
		);

		if (FAILED(hr)) { LastResult = "CoCreateInstance failed"; return nullptr; }

		Microsoft::WRL::ComPtr<IWICBitmapDecoder> pDecoder = nullptr;

		auto wstr = Util::ToWString(filename);
		hr = pFactory->CreateDecoderFromFilename(wstr.c_str(),
			nullptr,
			GENERIC_READ,
			WICDecodeMetadataCacheOnDemand,
			&pDecoder);

		if (FAILED(hr)) { LastResult = "CreateDecoderFromFilename failed"; return nullptr; }

		Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> pFrame = nullptr;

		hr = pDecoder->GetFrame(0u, &pFrame);

		if (FAILED(hr)) { LastResult = "GetFrame failed"; return nullptr; }

		hr = pFrame->GetSize(&width, &height);

		if (FAILED(hr)) { LastResult = "GetSize failed"; return nullptr; }

		Microsoft::WRL::ComPtr<IWICFormatConverter> pConverter = nullptr;

		hr = pFactory->CreateFormatConverter(&pConverter);

		if (FAILED(hr)) { LastResult = "CreateFormatConverter failed"; return nullptr; }

		// for reference
		// DXGI_FORMAT_R8G8B8A8_SNORM      GUID_WICPixelFormat32bppRGBA
		// DXGI_FORMAT_R32G32B32A32_FLOAT  GUID_WICPixelFormat128bppRGBAFloat

		hr = pConverter->Initialize(pFrame.Get(),
			GUID_WICPixelFormat128bppRGBAFloat,
			WICBitmapDitherTypeNone,
			nullptr,
			0.0f,
			WICBitmapPaletteTypeCustom);

		if (FAILED(hr)) { LastResult = "Initialize failed"; return nullptr; }

		// storing the image in the BECanvas member
		Surface2D<DirectX::XMVECTOR> surface(width, height); // using XMVECTOR for the 4x32 float size

		hr = pConverter->CopyPixels(nullptr,
			surface.GetPitch(),
			surface.GetTotalBytes(),
			(byte*)surface.GetData());

		if (FAILED(hr)) { LastResult = "CopyPixels failed"; return nullptr; }

		auto texture = std::make_shared<Texture>(0, surface, DXGI_FORMAT_R32G32B32A32_FLOAT);

		texture->SetName(filename);

		return texture;
	}
}
