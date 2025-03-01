/**
 * @file    Buffers.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    01/03/25
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include <d3d11_2.h>

class GraphicsBuffers {
 public:
  GraphicsBuffers() = default;
  virtual ~GraphicsBuffers() {
    SAFE_RELEASE(m_pBuffer);
  };

 public:
 //protected:
  friend class GraphicsAPI;
  ID3D11Buffer* m_pBuffer = nullptr;
};