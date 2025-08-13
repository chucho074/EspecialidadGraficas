/**
 * @file    Material.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    04/06/2025
 */

/**
 * @include
 */
 #include "Material.h"

void 
BaseMaterial::setTexture(TextureRef inTexture, 
                         TEXTURE_TYPE::E inType) {
  auto& texManager = g_textureManager();
  if(!m_textures.empty()) {
    auto iterTexture = m_textures.find(inType);
    if(iterTexture != m_textures.end()) {
      // If the texture is already set, remove it
      if(iterTexture->second == inTexture) {
        return; // No need to set the same texture again
      }
      // If the texture is a default or missing texture, remove it
      if(iterTexture->second == texManager.getDefaultTexture() ||
         iterTexture->second == texManager.getDefaultNormalTexture() ||
         iterTexture->second == texManager.getMissingTexture()) {
        m_textures.erase(iterTexture);
      }
    }    
  }
  m_textures.insert({inType, inTexture});
}

SPtr<Texture> 
BaseMaterial::getTexture(TEXTURE_TYPE::E inType) const {
  auto iter = m_textures.find(inType);

  if(iter != m_textures.end()) {
    return g_textureManager().getTexture(iter->second);
  }
  
}

void 
BaseMaterial::draw() {
  auto& GAPI = g_graphicsAPI();
  auto& shadeManager = g_shaderManager();
  auto& texManager = g_textureManager();

  if(m_shader.shaderID != UID::ZERO) {
    shadeManager.setDataToShader(m_shader);
  }

  GAPI.setShaderResource(0, getTexture(TEXTURE_TYPE::kAlbedo));
  
}

////////////////////////////////////////////////////////////////////////////////////////////


PBRMaterial::PBRMaterial() {

  auto& texManager = g_textureManager();

  setTexture(texManager.getDefaultTexture(), TEXTURE_TYPE::kAlbedo);
  setTexture(texManager.getDefaultTexture(), TEXTURE_TYPE::kRoughness);
  setTexture(texManager.getDefaultTexture(), TEXTURE_TYPE::kMetallic);
  setTexture(texManager.getDefaultNormalTexture(), TEXTURE_TYPE::kNormal);

  m_albedoColor = {1.f, 1.f, 1.f};
  m_emissiveColor = {0.f, 0.f, 0.f};
  m_metalic = 0.5f;
  m_roughness = 0.5f;
  m_ambientOclussion = 0.5f;
  m_opacity = 0.5f;
  m_gloss = 0.5f;
  m_specular = 0.5f;

}

void
PBRMaterial::draw() {
  auto& GAPI = g_graphicsAPI();
  auto& texManager = g_textureManager();
  auto& shadeManager = g_shaderManager();


  if(m_shader.shaderID != UID::ZERO) {
    shadeManager.setDataToShader(m_shader);
  }
  
  GAPI.setShaderResource(0, getTexture(TEXTURE_TYPE::kAlbedo));
  GAPI.setShaderResource(1, getTexture(TEXTURE_TYPE::kNormal));
  GAPI.setShaderResource(2, getTexture(TEXTURE_TYPE::kRoughness));
  GAPI.setShaderResource(3, getTexture(TEXTURE_TYPE::kMetallic));

}