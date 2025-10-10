/**
 * @file    App.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    29/09/2025
 * @brief   .
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "SceneGraph.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "giTime.h"
#include "Module.h"
#include "Prop.h"

#include <SDL3/SDL.h>
#include "UI.h"



class App : public Module<App>{
 public:
  App();
  ~App();

  bool init(void * inHandle);
  void update(float inDeltaTime);
  void render();
  void destroy();

  virtual void onCreate();
  virtual void onUpdate(float inDeltaTime);
  virtual void onRender();
  virtual void onDestroy();
 
 protected:
  bool
  initSystems();

  void 
  createShaders();

  void
  reloadTextures(); //Renderer

  void
  setInitialModels();
   
 public:
  Vector2i m_windowSize = {1280 , 720};

  SceneGraph* m_pSceneGraph = nullptr;
  GraphicsAPI* m_pGAPI = nullptr;
  ShaderManager* m_pShaderManager = nullptr;
  TextureManager* m_pTextureManager = nullptr;
  
  ShaderRef m_GBufferShaderRef;
  ShaderRef m_LightShaderRef;
  ShaderRef m_ShadowShaderRef;

  SPtr<Texture> m_renderPassRT;

  Vector<SPtr<Texture>> gbuffer;
  SPtr<Texture> m_dsShadowMap;

  giTime m_appTime;

  Vector2 m_viewportSize = Vector2::ZERO;
  Vector2 m_prevViewportSize = Vector2::ZERO;


  SPtr<Prop> m_pDinoActor;
  SPtr<Prop> m_pTerrainActor;
  SPtr<LightProp> m_pLightActor;
  
  SPtr<UI> m_UI;
 private:
  bool g_vsync = true;
  void* m_handle = nullptr;

 public:
  MatrixCollection m_WVP;


};

App& 
g_app();