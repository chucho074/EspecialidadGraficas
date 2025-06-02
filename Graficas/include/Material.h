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
  BaseMaterial() {
    m_albedo = make_shared<Texture>();
  }

  ~BaseMaterial() = default;

  void
  setAlbedo(const Path& inPath) {
    Image tmpAlbedo;
    tmpAlbedo.decode(inPath);
    m_albedo->createFromImage(tmpAlbedo);
  }

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
  PBRMaterial() {
    m_normalTexture = make_shared<Texture>();
    m_roughnessTexture = make_shared<Texture>();
    m_metalicTexture = make_shared<Texture>();

    setAlbedo("Models/DefaultTexture.bmp");
    setRoughnessTexture("Models/DefaultTexture.bmp");
    setMetalicTexture("Models/DefaultTexture.bmp");
    setNormalTexture("Models/DefaultNormal.bmp");

    m_albedoColor = {1.f, 1.f, 1.f};
    m_emissiveColor = {0.f, 0.f, 0.f};
    m_metalic = 0.5f;
    m_roughness = 0.5f;
    m_ambientOclussion = 0.5f;
    m_opacity = 0.5f;
    m_gloss = 0.5f;
    m_specular = 0.5f;
    
  };

  ~PBRMaterial() = default;
  
  ////////////////////////////////////////////////////////////////////////////////////////////

  void
  setNormalTexture(const Path& inPath) {
    Image tmpNormal;
    tmpNormal.decode(inPath);
    m_normalTexture->createFromImage(tmpNormal);
  }

  void
  setRoughnessTexture(const Path& inPath) {
    Image tmpRoughness;
    tmpRoughness.decode(inPath);
    m_roughnessTexture->createFromImage(tmpRoughness);
  }

  void
  setMetalicTexture(const Path& inPath) {
    Image tmpMetalic;
    tmpMetalic.decode(inPath);
    m_metalicTexture->createFromImage(tmpMetalic);
  }

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
  draw() override {
    auto& GAPI = g_graphicsAPI();
    GAPI.setShaderResource(0, m_albedo);

    if(m_normalTexture) {
      GAPI.setShaderResource(1, m_normalTexture);
    }
    if(m_roughnessTexture) {
      GAPI.setShaderResource(2, m_roughnessTexture);
    }
    if(m_metalicTexture) {
      GAPI.setShaderResource(3, m_metalicTexture);
    }
  }


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

////////////////////////////////////////////////////////////////////////////////////////////