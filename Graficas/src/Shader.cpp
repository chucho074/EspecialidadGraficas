#include "Shader.h"

Shader::~Shader() {
  SAFE_RELEASE(m_pBlob);
}

bool 
Shader::compile(const Path& inFilePath, 
                const String& inEntryFunction, 
                const String& inShaderModel) {
  fstream shaderFile(inFilePath, ios::in | ios::ate);
  if(!shaderFile.is_open()) {
    return false;
  }
  size_T fileSize = shaderFile.tellg();
  shaderFile.seekg(0, ios::beg);

  Vector<char> vShaderCode(fileSize);

  shaderFile.read(vShaderCode.data(), fileSize);

  uint32 flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
  flags |= D3DCOMPILE_DEBUG;
#endif

  ID3DBlob* pErrorBlob = nullptr;


  HRESULT hr = D3DCompile(vShaderCode.data(),
                          vShaderCode.size(),
                          nullptr,
                          nullptr,
                          nullptr,
                          inEntryFunction.c_str(),
                          inShaderModel.c_str(),
                          flags,
                          0,
                          &m_pBlob,
                          &pErrorBlob);
  if(FAILED(hr)) {
    MessageBoxA(nullptr, (LPCSTR)pErrorBlob->GetBufferPointer(), "Error", MB_OK);
    SAFE_RELEASE(pErrorBlob);
    return false;
  }

  return true;
}

VertexShader::~VertexShader(){
  SAFE_RELEASE(m_pVertexShader);
}

PixelShader::~PixelShader() {
  SAFE_RELEASE(m_pPixelShader);
}
