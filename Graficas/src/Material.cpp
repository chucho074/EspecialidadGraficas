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

BaseMaterial::BaseMaterial() {
  m_albedo = make_shared<Texture>();
}

void
BaseMaterial::setAlbedo(const Path& inPath) {
  Image tmpAlbedo;
  tmpAlbedo.decode(inPath);
  m_albedo->createFromImage(tmpAlbedo);
}

 ////////////////////////////////////////////////////////////////////////////////////////////


PBRMaterial::PBRMaterial() {
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

}

void
PBRMaterial::setNormalTexture(const Path& inPath) {
  Image tmpNormal;
  tmpNormal.decode(inPath);
  m_normalTexture->createFromImage(tmpNormal);
}

void
PBRMaterial::setRoughnessTexture(const Path& inPath) {
  Image tmpRoughness;
  tmpRoughness.decode(inPath);
  m_roughnessTexture->createFromImage(tmpRoughness);
}

void
PBRMaterial::setMetalicTexture(const Path& inPath) {
  Image tmpMetalic;
  tmpMetalic.decode(inPath);
  m_metalicTexture->createFromImage(tmpMetalic);
}

void
PBRMaterial::draw() {
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