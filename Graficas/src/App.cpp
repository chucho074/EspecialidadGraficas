/**
 * @file    App.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    29/09/2025
 * @brief   .
 */

/**
 * @include
 */
#include "App.h"
#include "Configs.h"

App::App() {
  
}

App::~App() {
  destroy();
}

bool 
App::init(void* inHandle) {
  if(!inHandle) {
    return false;
  }
  m_handle = inHandle;

  if(!initSystems()) {
    SDL_Log("App::init() > Failed to initialize the systems.");
    return false;
  }

  //m_shadowCamera = make_shared<Camera>();
  m_dsShadowMap = make_shared<Texture>();
  m_UI = make_shared<UI>();

  onCreate();

  m_WVP.world.identity();
  m_WVP.time = 1.f;
  m_WVP.lightIntensity = 1.f;
  m_WVP.lightRadius = 250.f;

  //m_WVP.lightView = m_shadowCamera->getViewMatrix();

  //m_WVP.lightProjection = m_shadowCamera->getOrthoMatrix();
  m_WVP.lightColor = Vector3(1.f, 1.f, 1.f);

  m_WVP.lightColor = {1.f, 1.f, 1.f};
  m_WVP.lightIntensity = 1.f;
  m_WVP.lightRadius = 250.f;
  //m_WVP.lightPosition = m_shadowCamera->getPosition();

  setInitialModels();

  //Resizetextures?

  m_UI->init(inHandle);
}

void App::update(float inDeltaTime) {
  auto& cameraRef = m_pSceneGraph->m_editorCamera;
  m_appTime.update();
  cameraRef->move(m_appTime.getTime());
  m_pSceneGraph->update(m_appTime.getTime());

  onUpdate(inDeltaTime);

}

void 
App::render() {
  auto& cameraRef = m_pSceneGraph->m_editorCamera;

  //Rellenar el input assembly
  //IA > Input Assambly
  //OM > Output Merger

  //m_pLightActor->setPosition(m_shadowCamera->getPosition());

  //Set the viewport
  {
    D3D11_VIEWPORT vp;
    vp.Width = m_windowSize.x;
    vp.Height = m_windowSize.y;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pGAPI->setViewport(vp);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////

  //Reset Values and clear RTs & DSVs
  {
    m_pGAPI->setRenderTargets(m_pGAPI->m_pBackBufferRTV,
                              m_pGAPI->m_pBackBufferDSV);

    FloatColor clearColor = {0.5f, 0.5f, 1.0f, 1.0f};
    FloatColor blackClearColor = {0.0f, 0.f, 0.0f, 0.0f};

    m_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, m_pGAPI->m_pBackBufferDSV);

    m_pGAPI->clearRTV(m_pGAPI->m_pBackBufferRTV, clearColor);
    m_pGAPI->clearRTV(m_renderPassRT, clearColor);
    m_pGAPI->clearDSV(m_pGAPI->m_pBackBufferDSV);
    m_pGAPI->clearDSV(m_dsShadowMap);

    for(auto tex : gbuffer) {
      m_pGAPI->clearRTV(tex, blackClearColor);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Values for cbuffer
  static float rotationAngle = 0.f;
  static float tempo = 0.f;
  tempo += 0.1f;
  m_WVP.time = tempo;

  m_WVP.world.identity();
  m_WVP.view = cameraRef->getViewMatrix();
  m_WVP.projection = cameraRef->getProjectionMatrix();

  //m_WVP.lightView = m_shadowCamera->getViewMatrix();
  //m_WVP.lightProjection = m_shadowCamera->getOrthoMatrix();

  g_shaderManager().setConstantValues(m_WVP);

  ////////////////////////////////////////////////////////////////////////////////////////////  Shadow Pass

  {

    m_pShaderManager->setConstantValues(m_WVP);
    m_pShaderManager->setDataToShader(m_ShadowShaderRef);

    Vector<SPtr<Texture>> rt = {
      nullptr,
      nullptr,
      nullptr
    };

    m_pGAPI->setRenderTargets(3, rt, m_dsShadowMap);

    m_pSceneGraph->draw(false);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Draw in GBuffer

  {
    m_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, m_pGAPI->m_pBackBufferDSV);

    m_pSceneGraph->draw(true);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Light Pass

  {

    m_pShaderManager->setConstantValues(m_WVP);
    m_pShaderManager->setDataToShader(m_LightShaderRef);
    m_WVP.viewPos = cameraRef->getPosition();
    m_pShaderManager->setConstantValues(m_WVP);

    Vector<SPtr<Texture>> rt = {
      m_renderPassRT,
      nullptr,
      nullptr
    };

    m_pGAPI->setRenderTargets(3, rt, nullptr);

    m_pGAPI->setShaderResource(0, gbuffer[0]);
    m_pGAPI->setShaderResource(1, gbuffer[1]);
    m_pGAPI->setShaderResource(2, gbuffer[2]);
    m_pGAPI->setShaderResource(3, nullptr);  //AO map here
    m_pGAPI->setShaderResource(4, m_dsShadowMap);

    m_pGAPI->m_pDeviceContext->Draw(3, 0);

    // Reset the values to nulls / defaults.
    m_pGAPI->setShaderResource(0, nullptr);
    m_pGAPI->setShaderResource(1, nullptr);
    m_pGAPI->setShaderResource(2, nullptr);
    m_pGAPI->setShaderResource(3, nullptr);
    m_pGAPI->setShaderResource(4, nullptr);

    rt = {
      m_pGAPI->m_pBackBufferRTV,
      nullptr,
      nullptr
    };

    m_pGAPI->setRenderTargets(3, rt, nullptr);
  }

  onRender();

  m_UI->render();
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Present

  m_pGAPI->present(g_configs().s_vsync);  //For now, is in the main, because of imgui code

}

void 
App::destroy() {
  
  m_UI->destroy();

  GraphicsAPI::shutDown();
  ShaderManager::shutDown();
  TextureManager::shutDown();
  
  m_pSceneGraph = nullptr;
  m_pGAPI = nullptr;
  m_pShaderManager = nullptr;
  m_pTextureManager = nullptr;  

  onDestroy();

}

void App::onCreate() {

}

void App::onUpdate(float inDeltaTime) {

}

void App::onRender() {

}

void App::onDestroy() {

}

bool 
App::initSystems() {
  GraphicsAPI::startUp();
  GraphicsAPI* GAPI = new GraphicsAPI(m_handle);
  g_graphicsAPI().setObject(GAPI);
  m_pGAPI = &g_graphicsAPI();

  if(!m_pGAPI) {
    return false;
  }

  //Initialize the ShaderManager
  ShaderManager::startUp();
  ShaderManager* shMan = new ShaderManager();
  g_shaderManager().setObject(shMan);
  m_pShaderManager = &g_shaderManager();

  createShaders();

  TextureManager::startUp();
  TextureManager* texMan = new TextureManager();
  g_textureManager().setObject(texMan);
  m_pTextureManager = &g_textureManager();
  m_pTextureManager->init();

  SceneGraph::startUp();
  SceneGraph* scnGraph = new SceneGraph();
  g_sceneGraph().setObject(scnGraph);
  m_pSceneGraph = &g_sceneGraph();
  m_pSceneGraph->init();

  m_WVP.view = m_pSceneGraph->m_editorCamera->getViewMatrix();
  m_WVP.projection = m_pSceneGraph->m_editorCamera->getProjectionMatrix();
  m_WVP.viewPos = m_pSceneGraph->m_editorCamera->getPosition();
  
}

void 
App::createShaders() {
  m_ShadowShaderRef = m_pShaderManager->createShaderProgram(g_configs().s_shadowShaderPath,
                                                            g_configs().s_shadowEntryPointVS,
                                                            g_configs().s_shadowEntryPointPS);

  g_shaderManager().setSamplerToShader(m_ShadowShaderRef, SAMPLER_USAGE::kAll);

  m_GBufferShaderRef = m_pShaderManager->createShaderProgram(g_configs().s_gbufferShaderPath,
                                                             g_configs().s_gbufferEntryPointVS,
                                                             g_configs().s_gbufferEntryPointPS);

  g_shaderManager().setSamplerToShader(m_GBufferShaderRef, SAMPLER_USAGE::kAll);

  g_shaderManager().setDefaultShader(m_GBufferShaderRef);

  m_LightShaderRef = m_pShaderManager->createShaderProgram(g_configs().s_lightShaderPath,
                                                           g_configs().s_lightEntryPointVS,
                                                           g_configs().s_lightEntryPointPS);

  g_shaderManager().setSamplerToShader(m_LightShaderRef, SAMPLER_USAGE::kAll);
  g_shaderManager().setRasterToShader(m_LightShaderRef, RASTER_USAGE::kCullFront);
}

void 
App::reloadTextures() {
  
}

void 
App::setInitialModels() {

  //Load models and textures
  {
    //m_pDinoActor = make_shared<Prop>();
    //m_pTerrainActor = make_shared<Prop>();
    //m_pLightActor = make_shared<LightProp>();

    ////////////////////////////////////////////////////////////////////////////////////////////  Dino
    m_pDinoActor = static_pointer_cast<Prop>(g_app().m_pSceneGraph->spawnActor<Prop>(g_app().m_pSceneGraph->getRoot(),
                                                                             Vector3(0, 0, 0), 
                                                                             Vector3(1, 1, 1)));
    
    //Rex model
    //if(!g_pDinoActor->m_model.loadFromBin("Models/rex_norm.bin")) {
    //if(!g_pDinoActor->m_model.loadFromBin("Models/BistroExt.bin")) {
    //if(!g_pDinoActor->m_model.loadFromBin("Models/R8_chico.bin")) {
    //if(!g_pDinoActor->m_model.loadFromBin("Models/bunny.bin")) {
    
    //if(!g_pDinoActor->createFromFile("Models/BistroExt.obj")) {
    
    
    //if(!g_pDinoActor->createFromFile("Models/Rex/Rex.gltf")) {
    if(!m_pDinoActor->createFromFile("Models/Rex/Rex_mat.obj")) {
    //if(!g_pDinoActor->createFromFile("Models/bistro/Exterior/exterior.obj")) {
    //if(!g_pDinoActor->createFromFile("D:/Biblioteca de chucho/Modelos/San_Miguel/san-miguel-low-poly.obj")) {
    //if(!g_pDinoActor->createFromFile("Models/BistroExt.obj")) {
    //if(!g_pDinoActor->createFromFile("Models/R8_chico.obj")) {
    //if(!g_pDinoActor->createFromFile("Models/bunny.obj")) {
      __debugbreak();
      return;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////  Terrain
    
    m_pTerrainActor = static_pointer_cast<Prop>(g_app().m_pSceneGraph->spawnActor<Prop>(g_app().m_pSceneGraph->getRoot(), 
                                                                                Vector3(0, 0, 0), 
                                                                                //Vector3(1.f, 1.f, 1.f)));
                                                                                Vector3(10.f, 10.f, 10.f)));
    
    //Disc model
    if(!m_pTerrainActor->createFromFile("Models/Plane.obj")) {
      __debugbreak();
      return;
    }
    
    
    
    ////////////////////////////////////////////////////////////////////////////////////////////  Light reference
    
    m_pLightActor = static_pointer_cast<LightProp>(g_app().m_pSceneGraph->spawnActor<LightProp>(g_app().m_pSceneGraph->getRoot(), 
                                                                                                Vector3(-10, 5, 0),
                                                                                                Vector3(1, 1, 1)));
  }
}

App& 
g_app() {
  return App::instance();
}