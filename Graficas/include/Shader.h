/**
 * @file    Shader.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    01/03/25
 * @brief   .
 */

/**
 * @include
 */

#pragma once
#include "PrerequisiteGraficas.h"
#include <d3d11_2.h>
#include <D3DCompiler.h>

class Shader
{
 public:
	Shader() = default;
	virtual ~Shader();

  /**
   * @brief       
   * @param       inFilePath 
   * @param       inEntryFunction 
   * @param       inShaderModel 
   * @return 
   */
  bool
  compile(const Path& inFilePath,
          const String& inEntryFunction,
          const String& inShaderModel);

ID3DBlob* getBlob() const {
  return m_pBlob;
}

 private:
  ID3DBlob* m_pBlob = nullptr;
};


class VertexShader final : public Shader {
 public: 
  VertexShader() = default;
  virtual ~VertexShader();

 public:
 //protected:
  friend class GraphicsAPI;
  ID3D11VertexShader* m_pVertexShader = nullptr;
};

class PixelShader final : public Shader {
 public: 
  PixelShader() = default;
  virtual ~PixelShader();

 public: 
 //protected: 
  friend class GraphicsAPI;
  ID3D11PixelShader* m_pPixelShader = nullptr;
};