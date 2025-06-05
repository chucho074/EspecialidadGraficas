/**
 * @file    Material.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    01/6/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Texture.h"
#include "GraphicsAPI.h"

class BaseMaterial {
 public:
  BaseMaterial();

  ~BaseMaterial() = default;

  void
  setAlbedo(const Path& inPath);

  void
  setTexture(SPtr<Texture> inTexture) {
    m_albedo = inTexture;
  }

  WPtr<Texture>
  getAlbedo() const {
    return m_albedo;
  }

  virtual void 
  draw() {
    auto& GAPI = g_graphicsAPI();
    GAPI.setShaderResource(0, m_albedo);
  }


 protected:
  SPtr<Texture> m_albedo;

  Vector3 m_albedoColor = Vector3::ZERO;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class PBRMaterial : public BaseMaterial {
 public:
  PBRMaterial();

  ~PBRMaterial() = default;
  
  ////////////////////////////////////////////////////////////////////////////////////////////

  void
  setNormalTexture(const Path& inPath);

  void
  setRoughnessTexture(const Path& inPath);

  void
  setMetalicTexture(const Path& inPath);

  ////////////////////////////////////////////////////////////////////////////////////////////

  void
  setNormalTexture(SPtr<Texture> inNormalTexture) {
    m_normalTexture = inNormalTexture;
  }

  void
  setRoughnessTexture(SPtr<Texture> inRoughnessTexture) {
    m_roughnessTexture = inRoughnessTexture;
  }

  void
  setMetalicTexture(SPtr<Texture> inMetalicTexture) {
    m_metalicTexture = inMetalicTexture;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////

  WPtr<Texture>
  getNormalTexture() const {
    return m_normalTexture;
  }

  WPtr<Texture>
  getRoughnessTexture() const {
    return m_roughnessTexture;
  }

  WPtr<Texture>
  getMetalicTexture() const {
    return m_metalicTexture;
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  
  void
  draw() override;


  ////////////////////////////////////////////////////////////////////////////////////////////
 protected:
  SPtr<Texture> m_normalTexture;
  SPtr<Texture> m_roughnessTexture;
  SPtr<Texture> m_metalicTexture;

  Vector3 m_emissiveColor    = Vector3::ZERO;
  float   m_metalic          = 0.f;
  float   m_roughness        = 0.f;
  float   m_ambientOclussion = 0.f;
  float   m_opacity          = 0.f;
  float   m_gloss            = 0.f;
  float   m_specular         = 0.f;

};

//////////////////////////////////////////////////////////////////////////////////////////////