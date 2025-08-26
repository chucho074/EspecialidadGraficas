/**
 * @file    TextureManager.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    11/08/2025
 */

/**
 * @include
 */
#include "TextureManager.h"
#include "GraphicsAPI.h"

void 
TextureManager::init() {
  m_missingTexture = loadTexture("Models/missingTextureV2.png");
  if(!m_missingTexture.id) {
    __debugbreak();
  }
  m_defaultTexture = loadTexture("Models/DefaultTexture.png");
  //m_defaultTexture = loadTexture("Models/DefaultTexture.png"); // Verify if this is a duplicate texture
  m_defaultNormalTex = loadTexture("Models/DefaultNormal.png");
}

TextureRef 
TextureManager::loadTexture(const Path& inFilePath) {
  
  if(inFilePath.empty()) {
    //ConsoleOut << "The filePath was empty when trying to load a texture" << ConsoleLine;
    return TextureRef();
  }

  auto& gapi = g_graphicsAPI();

  auto tmpIter = m_readedFiles.find(inFilePath);
  if(tmpIter != m_readedFiles.end()) { // If the texture is already readed
    // If the texture is already loaded, return it.
    return tmpIter->second;
  }
  else { // If the texture is not loaded yet
    TextureRef outRef;
    SPtr<Texture> newTexture = make_shared<Texture>();
    outRef.id = UID();
    outRef.name = inFilePath.stem().string();
    
    newTexture->createFromFile(inFilePath);

    // Save the texture reference
    m_textures.insert({outRef, newTexture});

    // Save the Path with the reference
    m_readedFiles.insert({inFilePath, outRef});
    
    return outRef;
  }

  ConsoleOut << "Can't load the texture: " << inFilePath << ConsoleLine;
  return m_missingTexture;
}

void 
TextureManager::UnloadTexture(TextureRef& inTextureID) {
  // Implementation for unloading a texture.
}

TextureManager&
g_textureManager() {
  return TextureManager::instance();
}