/**
 * @file    gbTexture.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    18/01/25
 * @brief   .
 */

/**
 * @include
 */
#include "Texture.h"
#include "GraphicsAPI.h"

//using std::min;

Texture::~Texture() {
  SAFE_RELEASE(m_pTexture);
  SAFE_RELEASE(m_pSRV);
  SAFE_RELEASE(m_pRTV);
  SAFE_RELEASE(m_pDSV);
}

void 
Texture::createFromImage(const Image& inImg) {
  m_image = inImg;
}

void
Texture::createFromImage(Image& inImg, const UPtr<GraphicsAPI>& inGAPI) {
  createFromImage(inImg);

  m_pTexture = inGAPI->createTexture(inImg.getWidth(), 
                                     inImg.getHeight(), 
                                     DXGI_FORMAT_B8G8R8A8_UNORM, 
                                     D3D11_USAGE_DEFAULT, 
                                     D3D11_BIND_SHADER_RESOURCE,
                                     0, 
                                     1,
                                     &m_pSRV);

  if(m_pTexture) {
    inGAPI->m_pDeviceContext->UpdateSubresource1(m_pTexture, 
                                                 0, 
                                                 nullptr, 
                                                 reinterpret_cast<char*>(inImg.getPixels()), 
                                                 inImg.getPitch(),
                                                 0, 
                                                 0);



    
  }

  

}

FloatColor 
Texture::sampleTexture(Vector2 inUVs, TEXTURE_ADDRESS::E inTexAddress, SAMPLE_FILTER::E inFilter) {
  
  switch (inFilter) {
  case SAMPLE_FILTER::kPOINT: {
    adjustTextureAddress(inTexAddress, inUVs);
    Vector2i tmpVect = {inUVs.x * (m_image.getWidth() - 1), 
                        inUVs.y * (m_image.getHeight() - 1) };
    return FloatColor(m_image.getPixel(tmpVect));
  }
  case SAMPLE_FILTER::kLINEAL: {
    adjustTextureAddress(inTexAddress, inUVs);
    float x = inUVs.x * m_image.getWidth() - 1;
    float y = inUVs.y * m_image.getHeight() - 1;
    
    int32 x0 = static_cast<int32> (x);
    int32 y0 = static_cast<int32> (y);
    int32 x1 = min(x0 + 1, m_image.getWidth() - 1);
    int32 y1 = min(y0 + 1, m_image.getHeight() - 1);

    float dx = x - x0;
    float dy = y - y0;
    //Color in 00
    FloatColor c00(m_image.getPixel({x0, y0}));
    FloatColor c10(m_image.getPixel({x1, y0}));
    FloatColor c01(m_image.getPixel({x0, y1}));
    FloatColor c11(m_image.getPixel({x1, y1}));

    //Interpolation time
    FloatColor c0 = c00 * (1.f - dx) + c10 * dx;
    FloatColor c1 = c01 * (1.f - dx) + c11 * dx;
    return c0 * (1.f - dy) + c1 * dy;

  }
  
  default:
    return FloatColor(0.f, 0.f, 0.f, 0.f);
    break;
  }
}

void 
Texture::adjustTextureAddress(TEXTURE_ADDRESS::E inTexAddress, 
                              Vector2& inUVs) {
  switch (inTexAddress) {
  
  case TEXTURE_ADDRESS::kBORDER: {
    
    break;
  }
  case TEXTURE_ADDRESS::kMIRROR_ONCE: {
    if((inUVs.x > -1.f && inUVs.x < 0.f) || (inUVs.x > 1.f && inUVs.x < 2.f)) {
      inUVs.x = 1.f - fmodf(inUVs.x, 1.f);
    }
    else {
      inUVs.x = inUVs.x < 0.f ? 0.f : inUVs.x;
      inUVs.x = inUVs.x > 1.f ? 1.f : inUVs.x;
    }

    if ((inUVs.y > -1.f && inUVs.y < 0.f) || (inUVs.y > 1.f && inUVs.y < 2.f)) {
      inUVs.y = 1.f - fmodf(inUVs.y, 1.f);
    }
    else {
      inUVs.y = inUVs.y < 0.f ? 0.f : inUVs.y;
      inUVs.y = inUVs.y > 1.f ? 1.f : inUVs.y;
    }

    break;
  }
  case TEXTURE_ADDRESS::kMIRROR: {
    inUVs.x = fmodf(inUVs.x, 2.f);
    inUVs.y = fmodf(inUVs.y, 2.f);

    if(inUVs.x < 0.f) {
      inUVs.x = 2.f + inUVs.x;
    }
    if (inUVs.y < 0.f) {
      inUVs.y = 2.f + inUVs.y;
    }
    if (inUVs.x > 1.f) {
      inUVs.x = 2.f - inUVs.x;
    }
    if (inUVs.y > 1.f) {
      inUVs.y = 2.f - inUVs.y;
    }

    break;
  }
  case TEXTURE_ADDRESS::kWRAP: {
    inUVs.x = fmodf(inUVs.x, 1.f);
    inUVs.y = fmodf(inUVs.y, 1.f);
    break;
  }
  default:
  case TEXTURE_ADDRESS::kCLAMP: {
    inUVs.x = inUVs.x < 0.0f ? 0.0f : inUVs.x;
    inUVs.x = inUVs.x > 1.0f ? 1.0f : inUVs.x;
    inUVs.y = inUVs.y < 0.0f ? 0.0f : inUVs.y;
    inUVs.y = inUVs.y > 1.0f ? 1.0f : inUVs.y;
    break;
  }
  }
}

void 
Texture::draw(Image& inData, 
              int32 inX, int32 inY, 
              Rect inRect, 
              TEXTURE_ADDRESS::E inAddressMode,
              BLEND_MODE::E inBlend) {
  if (inX >=  inData.getWidth() || inY >= inData.getHeight()) {
    return;
  }
  
  if(0 == inRect.EndX) inRect.EndX = inData.getWidth();
  if(0 == inRect.EndY) inRect.EndY = inData.getHeight();

  if(inX < 0) {
    inRect.IniX -= inX;
    inX = 0;
  }
  if(inY < 0) {
    inRect.IniY -= inY;
    inY = 0;
  }

  int32 realWidth = inRect.EndX - inRect.IniX;
  int32 realHeight = inRect.EndY - inRect.IniY;
  
  if (realWidth < 0 || realHeight < 0) {
    return;
  }

  if (inX + realWidth > inData.getWidth()) {
    realWidth -= (inX + realWidth) - inData.getWidth();
  }

  if(inY + realHeight > inData.getHeight()) {
    realHeight -= (inY + realHeight) - inData.getHeight();
  }

  for(int32 i = 0; i < realHeight; ++i) {
    for(int32 j = 0; j < realWidth; ++j) {
      
      FloatColor srcColor = sampleTexture({static_cast<float>(j) / realWidth, static_cast<float>(i) / realHeight}, inAddressMode);
      FloatColor dstColor = FloatColor(inData.getPixel({inX + j, inY + i}));
      
      //Esta es la lina donde se deberia de cambiar para el modo de blend.
      //FloatColor blendedColor = srcColor * srcColor.a + dstColor * (1.f - srcColor.a);
      FloatColor blendedColor;


      switch (inBlend)
      {
      case BLEND_MODE::kALPHA_BLEND: {
        blendedColor = srcColor * srcColor.a + dstColor * (1.f - srcColor.a);
        break;
      }
      case BLEND_MODE::kADDITIVE: {
        blendedColor = srcColor + dstColor;
        blendedColor.saturate();
        break;
      }
      case BLEND_MODE::kADDITIVE_PREMULT: {
        blendedColor = (srcColor * srcColor.a) + dstColor;
        blendedColor.saturate();
        break;
      }
      case BLEND_MODE::kMULTIPLICATIVE: {
        blendedColor = srcColor * dstColor;
        break;
      }
      case BLEND_MODE::kNONE:
      default: 
        blendedColor = srcColor;
        break;
      }
      
      inData.setPixel({inX + j, inY + i}, blendedColor.toColor());
      
    }
  }
}
