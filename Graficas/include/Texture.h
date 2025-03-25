/**
 * @file    Texture.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    15/03/25
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Image.h"
#include "Rect.h"
#include <d3d11_2.h>

namespace BLEND_MODE {
  enum E {
    kNONE = 0,
    kALPHA_BLEND,
    kADDITIVE,
    kADDITIVE_PREMULT,
    kMULTIPLICATIVE,
    kNUMOFMODES
  };
}

namespace SAMPLE_FILTER {
  enum E {
    kPOINT = 0,
    kLINEAL,
    kNUMOFSAMPLERS
  };
}

class GraphicsAPI;

class Texture
{
 public:
  Texture() = default;
  ~Texture();
  
  
  void 
  createFromImage(const Image& inImg);
  
  void 
  createFromImage(Image& inImg, const UPtr<GraphicsAPI>& inGAPI);

  FloatColor 
  sampleTexture(Vector2 inUVs, TEXTURE_ADDRESS::E inTexAddress = TEXTURE_ADDRESS::kCLAMP, SAMPLE_FILTER::E inFilter = SAMPLE_FILTER::kPOINT);
  
  void
  adjustTextureAddress(TEXTURE_ADDRESS::E inTexAddress, Vector2& inUVs);

  void
  draw(Image& inData, 
       int32 inX, int32 inY, 
       Rect inRect, 
       TEXTURE_ADDRESS::E inAddressMode = TEXTURE_ADDRESS::kCLAMP,
       BLEND_MODE::E inBlend = BLEND_MODE::kNONE);

  
  void
  clearTexture(float inClearColor[4], const UPtr<GraphicsAPI>& inGAPI);

  Image m_image;

  ID3D11Texture2D* m_pTexture = nullptr;
  ID3D11ShaderResourceView* m_pSRV = nullptr;
  ID3D11RenderTargetView* m_pRTV = nullptr;
  ID3D11DepthStencilView* m_pDSV = nullptr;

  Vector<Image> m_mips;

};