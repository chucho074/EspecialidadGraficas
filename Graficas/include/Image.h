/**
 * @file    gbImage.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    04/01/25
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Rect.h"

//using PixelShader = function<Color(float, float)>;


namespace TEXTURE_ADDRESS {
  enum E {
    kCLAMP = 0,
    kWRAP,
    kMIRROR,
    kBORDER,
    kMIRROR_ONCE,
    kNUMOFTYPES
  };
}

namespace RegionCode {
  enum E {
    kINSIDE = 0,
    kLEFT = 1,
    kRIGHT = 2,
    kBOTTOM = 4,
    kTOP = 8,
  };
}

class Image {
 public:
  Image() = default;
  ~Image() {
    if (m_pixels) {
      //delete[] m_pixels;
    }
  };

  void
  create(int32 inWidth, int32 inHeight, int32 inbpp) {
    m_width = inWidth;
    m_height = inHeight;
    m_bpp = inbpp;
    m_pixels = reinterpret_cast<unsigned char*>(new char[getPitch() * getHeight()]);
  }
  
  void
  decode(Path inFilePath);

  void
  encode(Path inFilePath);

  void
  clearColor(const Color& inColor);

  /**
   * @brief       Set the bit information of a given img.
   * @param       src               The image to copy. 
   * @param       inPos             The position to set the image. 
   * @param       inRect            The rect to take of the image (src).
   * @param       inColorKey        The color to omit (transparency).
   */
  void
  bitBlt(Image& src, 
         Vector2i inPos, 
         Rect inRect = Rect::Zero,
         Color* inColorKey = nullptr);


  void
  line(Vector2i inIniPos, Vector2i inEndPos, const Color& inColor);

  void
  bresenhamline(Vector2i inIniPos, Vector2i inEndPos, const Color& inColor);

  void
  bresenhamCircle(Vector2i inPos, int32 inRadius, const Color& inColor);

  int32
  getWidth() const {
    return m_width;
  }

  int32 
  getHeight() const {
    return m_height;
  }

  int32 
  getBpp() const {
    return m_bpp >> 3;
  }

  int32 
  getPitch() const {
    return m_width * getBpp(); //Densidad de informacion por linea
  }

  uint8*
  getPixels() {
    return m_pixels;
  }

  Color 
  getPixel(Vector2i inPos);

  void 
  setPixel(Vector2i inPos, const Color& inColor);

  int32 
  computeRegionCode(Vector2i inPos, int32 xMin, int32 yMin, int32 xMax, int32 yMax) {
    int32 code = RegionCode::kINSIDE;
    if (inPos.x < xMin) { code |= RegionCode::kLEFT; }
    else if (inPos.x > xMax) { code |= RegionCode::kRIGHT; }

    if (inPos.y < yMin) { code |= RegionCode::kBOTTOM; }
    else if (inPos.y > yMax) { code |= RegionCode::kTOP; }

    return code;
  }

  bool
  clipLine(Vector2i& inIniPos, Vector2i& inEndPos, const Color& inColor);


 public:
  //Resolution
  int32 m_width;
  int32 m_height;
  int32 m_bpp; //Bits per pixel

  unsigned char * m_pixels = nullptr; //in bytes
};
