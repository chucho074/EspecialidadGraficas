/**
 * @file    TextureManager.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    11/08/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "UUID.h"
#include "Texture.h"
#include "Module.h"

struct TextureRef {
  UID id = UID::ZERO;
  String name;

  operator bool() const {
    return id != UID::ZERO;
  }

  bool operator==(const TextureRef& other) const {
    return id == other.id;
  }

  bool operator!=(const TextureRef& other) const {
    return id != other.id;
  }

};

// Especializacion de hash para TextureRef
namespace std {
  template <>
  struct hash<TextureRef> {
    size_T operator()(const TextureRef& tex) const {
      return hash<UID>()(tex.id);
    }
  };
}

class TextureManager : public Module<TextureManager>{
public:
  TextureManager() = default;
  ~TextureManager() = default;

  void
  init();

  TextureRef
  loadTexture(const Path& filePath);

  void 
  UnloadTexture(TextureRef& inTextureID);

  SPtr<Texture>&
  getTexture(TextureRef inTextureID) {
    auto iter = m_textures.find(inTextureID);
    if (iter != m_textures.end()) {
      return iter->second;
    }
    return getTexture(m_missingTexture);
  }

  TextureRef&
  getDefaultTexture() {
    return m_defaultTexture;
  }

  TextureRef&
  getDefaultNormalTexture() {
    return m_defaultNormalTex;
  }

  TextureRef&
  getMissingTexture() {
    return m_missingTexture;
  }

 //protected:
  UMap<TextureRef, SPtr<Texture>> m_textures;
  UMap<Path, TextureRef> m_readedFiles;

  TextureRef m_defaultTexture;
  TextureRef m_defaultNormalTex;
  TextureRef m_missingTexture;
};

TextureManager&
g_textureManager();