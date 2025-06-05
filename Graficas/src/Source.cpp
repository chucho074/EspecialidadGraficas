/**
 * @file    Source.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 */

/**
 * @include
 */
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <iostream>

#include "ShaderManager.h"
#include "PrerequisiteGraficas.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Texture.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "Prop.h"

Vector2 g_windowSize = {1280 , 720};


SDL_Window* g_pWindow = nullptr;
GraphicsAPI* g_pGAPI = nullptr;
ShaderManager* g_pShaderManager = nullptr;
SPtr<SceneGraph> g_pSceneGraph;

ShaderRef g_GBufferShaderRef;
ShaderRef g_LightShaderRef;

SPtr<Prop> g_pDinoActor;
SPtr<Prop> g_pTerrainActor;
SPtr<Prop> g_pBunnyActor;

Vector<SPtr<Texture>> gbuffer;

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {

  //Initialize sdl
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  //Create sdl window
  g_pWindow = SDL_CreateWindow("Graficas Basicas", 
                               g_windowSize.x, 
                               g_windowSize.y, 
                               SDL_WINDOW_RESIZABLE);

  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  auto pHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(g_pWindow), 
                                        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                        nullptr);
  //Initialize GraphicsAPI
  if(pHandle) {
    GraphicsAPI::startUp();
    GraphicsAPI* GAPI = new GraphicsAPI(reinterpret_cast<void*>(pHandle));
    g_graphicsAPI().setObject(GAPI);
    g_pGAPI = &g_graphicsAPI();

    if (!g_pGAPI) {
      return SDL_APP_FAILURE;
    }

  }
  
  //Create the SceneGraph
  g_pSceneGraph = make_shared<SceneGraph>();
  g_pSceneGraph->init();

  //Initialize the ShaderManager
  ShaderManager::startUp();
  ShaderManager* shMan = new ShaderManager();
  g_shaderManager().setObject(shMan);
  g_pShaderManager = &g_shaderManager();

  //Set camera info
  g_pSceneGraph->m_editorCamera.setLookAt(Vector3(-5, 5, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
  g_pSceneGraph->m_editorCamera.setPerspectiveHalf(3.1415926353f/4.f, g_windowSize, 0.1f, 1000.f);
  //g_Camera.setPerspective(75 * DEG2RAD, g_windowSize, 0.1f, 100.f);
  
  g_GBufferShaderRef = g_pShaderManager->createShaderProgram("Shaders/GBuffer.hlsl",
                                                             "gbuffer_vertex_main",
                                                             "gbuffer_pixel_main");

  g_LightShaderRef = g_pShaderManager->createShaderProgram("Shaders/lightShader.hlsl",
                                                           "vertex_main",
                                                           "pixel_main");

  //Load models and textures
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Dino
  //g_pDinoActor = static_pointer_cast<Prop>(g_pSceneGraph->spawnActor<Prop>(g_pSceneGraph->getRoot(),
  //                                                                        Vector3(0, 0, 0), 
  //                                                                        Vector3(1, 1, 1)));

  ////Rex model
  //if(!g_pDinoActor->m_model.loadFromFile("Models/rex_norm.obj")) {
  //  __debugbreak();
  //  return SDL_APP_FAILURE;
  //}

  //g_pDinoActor->m_material.setAlbedo("Models/Rex_C.bmp");
  //g_pDinoActor->m_material.setNormalTexture("Models/Rex_N.bmp");
  //g_pDinoActor->m_material.setRoughnessTexture("Models/Rex_R.bmp");
  //g_pDinoActor->m_material.setMetalicTexture("Models/Rex_M.bmp");
  //g_pDinoActor->m_material.setShaderRef(g_GBufferShaderRef);


  ////////////////////////////////////////////////////////////////////////////////////////////  Terrain
  
  g_pTerrainActor = static_pointer_cast<Prop>(g_pSceneGraph->spawnActor<Prop>(g_pSceneGraph->getRoot(),
                                                                             Vector3(0, 0, 0), 
                                                                             Vector3(0.08f, 0.08f, 0.08f)));

  //Disc model
  if(!g_pTerrainActor->m_model.loadFromFile("Models/disc.obj")) {
    return SDL_APP_FAILURE;
  }

  g_pTerrainActor->m_material.setAlbedo("Models/Terrain.bmp");
  g_pTerrainActor->m_material.setShaderRef(g_GBufferShaderRef);


  ////////////////////////////////////////////////////////////////////////////////////////////  GBuffer
  gbuffer.resize(3);
  for(int i = 0; i < gbuffer.size(); ++i) {
    gbuffer[i] = make_shared<Texture>();
  }
  //Pos
  gbuffer[0]->m_pTexture = g_pGAPI->createTexture(g_windowSize.x,
                                                  g_windowSize.y,
                                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &gbuffer[0]->m_pSRV,
                                                  &gbuffer[0]->m_pRTV);
  //Normals
  gbuffer[1]->m_pTexture = g_pGAPI->createTexture(g_windowSize.x,
                                                  g_windowSize.y,
                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &gbuffer[1]->m_pSRV,
                                                  &gbuffer[1]->m_pRTV);
  //Color
  gbuffer[2]->m_pTexture = g_pGAPI->createTexture(g_windowSize.x,
                                                  g_windowSize.y,
                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &gbuffer[2]->m_pSRV,
                                                  &gbuffer[2]->m_pRTV);

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  auto& shaderManager = g_shaderManager();
  auto& cameraRef = g_pSceneGraph->m_editorCamera;

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
  }

  if (event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_F5) {
      shaderManager.compileAllShaders();
    }
  }
  //Camera movement 

  //Get forward input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_W) {
      cameraRef.m_front = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_W) {
      cameraRef.m_front = false;
    }
  }
  //Get backard input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_S) {
      cameraRef.m_back = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_S) {
      cameraRef.m_back = false;
    }
  }
  //Get left input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_A) {
      cameraRef.m_left = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_A) {
      cameraRef.m_left = false;
    }
  }
  //Get right input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_D) {
      cameraRef.m_right = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_D) {
      cameraRef.m_right = false;
    }
  }
  //Get up input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_E) {
      cameraRef.m_up = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_E) {
      cameraRef.m_up = false;
    }
  }
  //Get down input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Q) {
      cameraRef.m_down = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Q) {
      cameraRef.m_down = false;
    }
  }

  //Rotate
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Z) {
      cameraRef.m_YawNeg = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Z) {
      cameraRef.m_YawNeg = false;
    }
  }
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_C) {
      cameraRef.m_YawPos = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_C) {
      cameraRef.m_YawPos = false;
    }
  }

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {

  auto& cameraRef = g_pSceneGraph->m_editorCamera;
  
  static Uint64 past = 0;
  Uint64 now = SDL_GetTicksNS();
  float deltaTime = (now - past) / 999999999.0f * 1000.0f;
  
  cameraRef.move(deltaTime);
  g_pSceneGraph->update(deltaTime);

  static float tempo = 0.f;
  tempo += 0.1f;

  //Rellenar el input assembly
  //IA > Input Assambly
  //OM > Output Merger
  
  //Set the viewport
  {
    D3D11_VIEWPORT vp;
    vp.Width = g_windowSize.x;
    vp.Height = g_windowSize.y;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;
    vp.TopLeftX = 0; 
    vp.TopLeftY = 0;
    g_pGAPI->setViewport(vp);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  
  //Reset Values and clear RTs & DSVs
  {
    g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, 
                              g_pGAPI->m_pBackBufferDSV);
    
    FloatColor clearColor = { 0.5f, 0.5f, 1.0f, 1.0f };
    FloatColor blackClearColor = { 0.0f, 0.f, 0.0f, 0.0f };

    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);

    g_pGAPI->clearRTV(g_pGAPI->m_pBackBufferRTV, clearColor);
    g_pGAPI->clearDSV(g_pGAPI->m_pBackBufferDSV);

    for(auto tex : gbuffer) {
      g_pGAPI->clearRTV(tex, blackClearColor);
    }

  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////  GBuffer Pass
  {
    
    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);

    g_pSceneGraph->draw();
    

  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////   Light Pass
  { 
    //Defferred shading pass

    MatrixCollection matrices;
    matrices.view = cameraRef.getViewMatrix();
    matrices.projection = cameraRef.getProjectionMatrix();
    matrices.viewDir = cameraRef.getViewDir();
    matrices.world = Matrix4::IDENTITY;
    matrices.time = tempo;


    g_pShaderManager->setDataToShader(g_LightShaderRef, matrices);

    Vector<SPtr<Texture>> rt = {
      g_pGAPI->m_pBackBufferRTV,
      nullptr,
      nullptr
    };

    g_pGAPI->setRenderTargets(3, rt, nullptr);

    g_pShaderManager->setRaster(RASTER_USAGE::kCullFront);

    g_pGAPI->setShaderResource(0, gbuffer[0]);
    g_pGAPI->setShaderResource(1, gbuffer[1]);
    g_pGAPI->setShaderResource(2, gbuffer[2]);
    g_pGAPI->setShaderResource(3, nullptr);

    g_pGAPI->m_pDeviceContext->Draw(3, 0);

    g_pGAPI->setShaderResource(0, nullptr);
    g_pGAPI->setShaderResource(1, nullptr);
    g_pGAPI->setShaderResource(2, nullptr);

  }

  g_pGAPI->m_pSwapChain->Present(1, 0);

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void
SDL_AppQuit(void* appstate, SDL_AppResult result) {

  /* SDL will clean up the window/renderer for us. */
  if(g_pWindow) {
    SDL_DestroyWindow(g_pWindow);
    g_pWindow = nullptr;
  }

  GraphicsAPI::shutDown();

  SDL_Quit();
}
