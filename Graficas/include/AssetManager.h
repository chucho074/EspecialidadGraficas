/**
 * @file    AssetManager.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    13/08/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "TextureManager.h"
#include "Module.h"

class AssetManager {
 public:
  AssetManager() = default;
  ~AssetManager() = default;
  
  /*void 
  loadTexture(const Path& filePath);
  
  void 
  UnloadTexture(const Path& textureName);
  
  SPtr<Texture> 
  GetTexture(const Path& textureName) const;*/

  /*MaterialRef
  CreateMaterial();

  TextureRef
  CreateTexture();*/

 
 protected:
  SPtr<TextureManager> m_textureManager;


 public: 
  friend class TextureManager;
};
