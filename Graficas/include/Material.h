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
#include "ShaderManager.h"
#include "TextureManager.h"

namespace TEXTURE_TYPE {
  enum E {
    kUnknown = 0,
    kAlbedo,
    kMetallic,
    kRoughness,
    kEmissive,
    kAmbientOclussion,
    kNormal,
    kGloss,
    kSpecular,
    kOpacity,
    kDisplacement,
    kSuportedTextureTypes
  };
}


class BaseMaterial {
 public:
  BaseMaterial() = default;
  ~BaseMaterial() = default;

  void
  setTexture(TextureRef inTexture, TEXTURE_TYPE::E inType);

  SPtr<Texture>
  getTexture(TEXTURE_TYPE::E inType) const;

  void
  changeTexture(TextureRef inNewTexture, TEXTURE_TYPE::E inType);

  virtual void 
  draw();
  
  TextureRef
  getTextureRef(TEXTURE_TYPE::E inTexType) {
    if(inTexType == TEXTURE_TYPE::kUnknown 
       || inTexType == TEXTURE_TYPE::kSuportedTextureTypes) {
      return g_textureManager().getMissingTexture();
    }

    auto iter = m_textures.find(inTexType);
    if(iter != m_textures.end()) {
      return iter->second;
    }

    return g_textureManager().getMissingTexture();
  }

  void
  setShaderRef(const ShaderRef& inShader) {
    m_shader = inShader;
  }

  ShaderRef
  getShaderRef() const {
    return m_shader;
  }

  // Setters for albedo color
  void 
  setAlbedoColor(const Vector3& inColor) { 
    m_albedoColor = inColor; 
  }

  // Getters for albedo color
  const Vector3& 
  getAlbedoColor() const { 
    return m_albedoColor; 
  }

  void
  setName(StringView& inName) {
    m_materialName = inName;
  }

  StringView
  getName() {
    return m_materialName;
  }


  Vector3 m_albedoColor = Vector3::ZERO;
  String m_materialName;

 protected:
  UMap<TEXTURE_TYPE::E, TextureRef> m_textures;
  ShaderRef m_shader;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class PBRMaterial : public BaseMaterial {
 public:
  PBRMaterial();

  ~PBRMaterial() = default;
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  
  void
  draw() override;

  //Setters for PBR properties
  void setEmissiveColor(const Vector3& inColor) { m_emissiveColor = inColor; }
  void setMetallic(float inMetalic) { m_metallic = inMetalic; }
  void setRoughness(float inRoughness) { m_roughness = inRoughness; }
  void setAmbientOclussion(float inAmbientOclussion) { m_ambientOclussion = inAmbientOclussion; }
  void setOpacity(float inOpacity) { m_opacity = inOpacity; }
  void setGloss(float inGloss) { m_gloss = inGloss; }
  void setSpecular(float inSpecular) { m_specular = inSpecular; }

  //Getters for PBR properties
  const Vector3& getEmissiveColor() const { return m_emissiveColor; }
  float getMetallic() const { return m_metallic; }
  float getRoughness() const { return m_roughness; }
  float getAmbientOclussion() const { return m_ambientOclussion; }
  float getOpacity() const { return m_opacity; }
  float getGloss() const { return m_gloss; }
  float getSpecular() const { return m_specular; }


 ////////////////////////////////////////////////////////////////////////////////////////////
 //protected:
  
  Vector3 m_emissiveColor    = Vector3::ZERO;
  float   m_diffuse          = 0.f; //?
  float   m_metallic          = 0.f;
  float   m_roughness        = 0.f;
  float   m_ambientOclussion = 0.f;
  float   m_opacity          = 0.f;
  float   m_gloss            = 0.f;
  float   m_specular         = 0.f;

};

//////////////////////////////////////////////////////////////////////////////////////////////