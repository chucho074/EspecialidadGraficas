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

#include "PrerequisiteGraficas.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Texture.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "Prop.h"

struct MatrixCollection {
  Matrix4 world;
  Matrix4 view;
  Matrix4 projection;
  Vector3 viewDir;

  float time;
};

Vector2 g_windowSize = {1280 , 720};

SPtr<SceneGraph> g_sceneGraph;

SDL_Window* g_pWindow = nullptr;
GraphicsAPI* g_pGAPI = nullptr;

UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;

UPtr<VertexShader> g_pGBufferVertexShader;
UPtr<PixelShader> g_pGBufferPixelShader;

UPtr<VertexShader> g_pDefferredVertexShader;
UPtr<PixelShader> g_pDefferredPixelShader;

UPtr<PixelShader> g_pPixelShader_Reflect;

ID3D11InputLayout* g_pInputLayout = nullptr;
ID3D11InputLayout* g_pDefInputLayout = nullptr;

UPtr<GraphicsBuffers> g_pVertexBuffer;
UPtr<GraphicsBuffers> g_pIndexBuffer;
UPtr<GraphicsBuffers> g_pCB_WVP;

ID3D11RasterizerState1* g_pRS_Default = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe = nullptr;
ID3D11RasterizerState1* g_pRS_Wireframe_NoCull = nullptr;
ID3D11RasterizerState1* g_pRS_CullFront = nullptr;

ID3D11SamplerState* g_pSS_Point = nullptr;
ID3D11SamplerState* g_pSS_Linear = nullptr;
ID3D11SamplerState* g_pSS_Anisotropic = nullptr;

MatrixCollection g_WVP;

EditorCamera g_Camera;

SPtr<Prop> g_pDinoActor;
SPtr<Prop> g_pTerrainActor;
SPtr<Prop> g_pBunnyActor;

SPtr<Texture> g_rtReflection;
SPtr<Texture> g_dsReflection;

Vector<SPtr<Texture>> gbuffer;

Transform g_worldTransform;

void recompileShaders() {
  auto pVertexShader = g_pGAPI->createVertexShaderFromFile("Shaders/basicVertexShader.hlsl",
                                                           "vertex_main");
  if(pVertexShader) {
    g_pVertexShader = std::move(pVertexShader);
  }


  auto pPixelShader = g_pGAPI->createPixelShaderFromFile("Shaders/basicVertexShader.hlsl",
                                                         "pixel_main");
  if(pPixelShader) {
    g_pPixelShader = std::move(pPixelShader);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  auto pGBufferVertexShader = g_pGAPI->createVertexShaderFromFile("Shaders/GBuffer.hlsl",
                                                           "gbuffer_vertex_main");
  if(pGBufferVertexShader) {
    g_pGBufferVertexShader = std::move(pGBufferVertexShader);
  }


  auto pGBufferPixelShader = g_pGAPI->createPixelShaderFromFile("Shaders/GBuffer.hlsl",
                                                         "gbuffer_pixel_main");
  if(pGBufferPixelShader) {
    g_pGBufferPixelShader = std::move(pGBufferPixelShader);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  auto pDefferredVertexShader = g_pGAPI->createVertexShaderFromFile("Shaders/lightShader.hlsl",
                                                           "vertex_main");
  if(pDefferredVertexShader) {
    g_pDefferredVertexShader = std::move(pDefferredVertexShader);
  }


  auto pDefferredPixelShader = g_pGAPI->createPixelShaderFromFile("Shaders/lightShader.hlsl",
                                                         "pixel_main");
  if(pDefferredPixelShader) {
    g_pDefferredPixelShader = std::move(pDefferredPixelShader);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////

  auto pPixelShader_Reflect = g_pGAPI->createPixelShaderFromFile("Shaders/basicVertexShader.hlsl",
                                                                 "pixel_reflect_main");
  if(pPixelShader_Reflect) {
    g_pPixelShader_Reflect = std::move(pPixelShader_Reflect);
  }

}


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

  auto pHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(g_pWindow), 
                                        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                        nullptr);
  if(pHandle) {
    //Initialize GraphicsAPI
    GraphicsAPI::startUp();
    GraphicsAPI* GAPI = new GraphicsAPI(reinterpret_cast<void*>(pHandle));
    g_graphicsAPI().setObject(GAPI);
    g_pGAPI = &g_graphicsAPI();

    if (!g_pGAPI) {
      return SDL_APP_FAILURE;
    }

    recompileShaders();
  }

  Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
  
  g_pInputLayout = g_pGAPI->createInputLayout(inputElementDescs, g_pVertexShader);

  g_pDefInputLayout = g_pGAPI->createInputLayout(inputElementDescs, g_pDefferredVertexShader);
  
  if (!g_pInputLayout) {
    return SDL_APP_FAILURE;
  }

  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  g_sceneGraph = make_shared<SceneGraph>();

  g_sceneGraph->init();

  //Set camera info
  g_Camera.setLookAt(Vector3(-5, 5, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
  g_Camera.setPerspectiveHalf(3.1415926353f/4.f, g_windowSize, 0.1f, 1000.f);
  //g_Camera.setPerspective(75 * DEG2RAD, g_windowSize, 0.1f, 100.f);
  
  //Set world info
  g_WVP.world.identity();
  g_WVP.view = g_Camera.getViewMatrix();
  g_WVP.projection = g_Camera.getProjectionMatrix();

  g_WVP.viewDir = g_Camera.getViewDir();
  g_WVP.time = 1.f;

  g_WVP.world.transpose();
  g_WVP.view.transpose();
  g_WVP.projection.transpose();

  //Set the reaster and sampler
  CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
  g_pRS_Default = g_pGAPI->createRasterState(descRD);

  descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
  descRD.CullMode = D3D11_CULL_FRONT;
  g_pRS_CullFront = g_pGAPI->createRasterState(descRD);

  descRD.FillMode = D3D11_FILL_WIREFRAME;
  descRD.CullMode = D3D11_CULL_NONE;
  g_pRS_Wireframe_NoCull = g_pGAPI->createRasterState(descRD);

  CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
  descSS.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  g_pSS_Point = g_pGAPI->createSamplerState(descSS);

  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  g_pSS_Linear = g_pGAPI->createSamplerState(descSS);

  descSS.Filter = D3D11_FILTER_ANISOTROPIC;
  descSS.MaxAnisotropy = 16; //Esto es lo que cambiamos en las opciones de los juegos
  g_pSS_Anisotropic = g_pGAPI->createSamplerState(descSS);



  //Create the WVP buffer 
  Vector<char> matrix_data;
  matrix_data.resize(sizeof(g_WVP));
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pCB_WVP = g_pGAPI->createConstantBuffer(matrix_data);

  /*CD3D11_RASTERIZER_DESC1 descRD(D3D11_DEFAULT);
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Default);

  descRD.FillMode = D3D11_FILL_WIREFRAME;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe);
  
  descRD.FillMode = D3D11_FILL_WIREFRAME;
  descRD.CullMode = D3D11_CULL_NONE;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_Wireframe_NoCull);

  descRD = CD3D11_RASTERIZER_DESC1(D3D11_DEFAULT);
  descRD.CullMode = D3D11_CULL_FRONT;
  g_pGAPI->m_pDevice->CreateRasterizerState1(&descRD, &g_pRS_CullFront);

  CD3D11_SAMPLER_DESC descSS(D3D11_DEFAULT);
  descSS.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
  descSS.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;

  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Point);

  descSS.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Linear);

  descSS.Filter = D3D11_FILTER_ANISOTROPIC;
  descSS.MaxAnisotropy = 16;
  g_pGAPI->m_pDevice->CreateSamplerState(&descSS, &g_pSS_Anisotropic);*/

  //Load models and textures
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Dino
  g_pDinoActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(), 
                                                                          Vector3(0, 0, 0), 
                                                                          Vector3(1, 1, 1)));

  //Rex model
  if(!g_pDinoActor->m_model.loadFromFile("Models/rex_norm.obj")) {
    __debugbreak();
    return SDL_APP_FAILURE;
  }

  g_pDinoActor->m_material.setAlbedo("Models/Rex_C.bmp");
  g_pDinoActor->m_material.setNormalTexture("Models/Rex_N.bmp");
  g_pDinoActor->m_material.setRoughnessTexture("Models/Rex_R.bmp");
  g_pDinoActor->m_material.setMetalicTexture("Models/Rex_M.bmp");


  ////////////////////////////////////////////////////////////////////////////////////////////  Terrain
  
  g_pTerrainActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(), 
                                                                             Vector3(0, 0, 0), 
                                                                             Vector3(0.08f, 0.08f, 0.08f)));

  //Disc model
  if(!g_pTerrainActor->m_model.loadFromFile("Models/disc.obj")) {
    return SDL_APP_FAILURE;
  }

  g_pTerrainActor->m_material.setAlbedo("Models/Terrain.bmp");

  g_rtReflection = make_shared<Texture>();
  g_dsReflection = make_shared<Texture>();

  ////////////////////////////////////////////////////////////////////////////////////////////  Bunny
  if(false){
    g_pBunnyActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(),
                                              Vector3(0, 0, 0),
                                              Vector3(0.08f, 0.08f, 0.08f)));
    
    if(!g_pBunnyActor->m_model.loadFromFile("Models/bunny.obj")) {
      __debugbreak();
      return SDL_APP_FAILURE;
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Reflect Tex
  
  //Reflection textures
  g_rtReflection->m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                      g_windowSize.y,
                                                      DXGI_FORMAT_B8G8R8A8_UNORM,
                                                      D3D11_USAGE_DEFAULT,
                                                      D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 
                                                      0,
                                                      1,
                                                      &g_rtReflection->m_pSRV,
                                                      &g_rtReflection->m_pRTV);

  g_dsReflection->m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                      g_windowSize.y,
                                                      DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                      D3D11_USAGE_DEFAULT,
                                                      D3D11_BIND_DEPTH_STENCIL, 
                                                      0,
                                                      1,
                                                      nullptr,
                                                      nullptr,
                                                      &g_dsReflection->m_pDSV);

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
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
  }

  if (event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_F5) {
      recompileShaders();
    }
  }
  //Camera movement 

  //Get forward input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_W) {
      g_Camera.m_front = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_W) {
      g_Camera.m_front = false;
    }
  }
  //Get backard input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_S) {
      g_Camera.m_back = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_S) {
      g_Camera.m_back = false;
    }
  }
  //Get left input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_A) {
      g_Camera.m_left = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_A) {
      g_Camera.m_left = false;
    }
  }
  //Get right input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_D) {
      g_Camera.m_right = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_D) {
      g_Camera.m_right = false;
    }
  }
  //Get up input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_E) {
      g_Camera.m_up = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_E) {
      g_Camera.m_up = false;
    }
  }
  //Get down input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Q) {
      g_Camera.m_down = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Q) {
      g_Camera.m_down = false;
    }
  }

  //Rotate
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Z) {
      g_Camera.m_YawNeg = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Z) {
      g_Camera.m_YawNeg = false;
    }
  }
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_C) {
      g_Camera.m_YawPos = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_C) {
      g_Camera.m_YawPos = false;
    }
  }

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {

  static Uint64 past = 0;
  Uint64 now = SDL_GetTicksNS();
  float deltaTime = (now - past) / 999999999.0f * 1000.0f;
  
  g_Camera.move(deltaTime);
  g_sceneGraph->update(deltaTime);

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
    g_rtReflection->clearTexture(blackClearColor.toArray());
    g_dsReflection->clearTexture(blackClearColor.toArray());

    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);

    g_pGAPI->clearRTV(g_pGAPI->m_pBackBufferRTV, clearColor);
    g_pGAPI->clearDSV(g_pGAPI->m_pBackBufferDSV);

    for(auto tex : gbuffer) {
      g_pGAPI->clearRTV(tex, blackClearColor);
    }

  }

  ////////////////////////////////////////////////////////////////////////////////////////////
  
  //Set Shaders info
  {
    g_pGAPI->setVertexShader(g_pGBufferVertexShader);
    g_pGAPI->setPixelShader(g_pGBufferPixelShader);

    g_pGAPI->setInputLayout(g_pInputLayout);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////
  static float rotationAngle = 0.f;
  //rotationAngle += 0.1f;

  //g_worldTransform.setRotation({0, rotationAngle, 0});

  static float tempo = 0.f;
  tempo += 0.1f;
  g_WVP.time = tempo;
  Vector<char> matrix_data;
  matrix_data.resize(sizeof(g_WVP));

  g_WVP.view = g_Camera.getViewMatrix();
  g_WVP.projection = g_Camera.getProjectionMatrix();
  g_WVP.view.transpose();
  g_WVP.projection.transpose();

  g_pGAPI->setConstantBuffer(0, g_pCB_WVP);

  ////////////////////////////////////////////////////////////////////////////////////////////
  
  //Set Rasterizer and samplers
  {
    g_pGAPI->setRasterState(g_pRS_Default);
    //g_pGAPI->setRasterState(g_pRS_Wireframe);

    //Set the samplers
    g_pGAPI->setSamplers(0, g_pSS_Point);
    g_pGAPI->setSamplers(1, g_pSS_Linear);
    g_pGAPI->setSamplers(2, g_pSS_Anisotropic);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Rex
  {
    g_pGAPI->setRasterState(g_pRS_Default);

    g_WVP.world = g_worldTransform.getMatrix() * g_pDinoActor->m_transform.getMatrix();
    g_WVP.world.transpose();
    
    memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
    g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

    //g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);
    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);

    g_pDinoActor->draw();
  }
  ////////////////////////////////////////////////////////////////////////////////////////////  Reflection
  if(false){
    Matrix4 translation3; //000
    translation3.identity();
    translation3.Translate({0, 0, 0});

    Matrix4 refScale; //Reflection
    refScale.identity();
    refScale.scale({1, -1, 1});
    g_WVP.world = refScale * g_worldTransform.getMatrix() * translation3;

    g_WVP.world.transpose();
    memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
    g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

    g_pGAPI->setRasterState(g_pRS_CullFront);

    g_pGAPI->clearSRV(1);
    
    g_pGAPI->setRenderTargets(g_rtReflection, g_dsReflection);
    
    g_pDinoActor->draw();
  }
  ////////////////////////////////////////////////////////////////////////////////////////////  Floor
  {
    g_WVP.world = g_worldTransform.getMatrix() * g_pTerrainActor->m_transform.getMatrix();
    g_WVP.world.transpose();
    memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
    g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

    g_pGAPI->setRasterState(g_pRS_Default);

    //g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);
    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);


    //g_pGAPI->setPixelShader(g_pPixelShader_Reflect);

    //g_pGAPI->setShaderResource(0, g_TerrainTexture);
    g_pGAPI->setShaderResource(1, g_rtReflection);

    g_pTerrainActor->draw();

  }
  ////////////////////////////////////////////////////////////////////////////////////////////  Bunny
  if (false){
    g_pGAPI->setPixelShader(g_pGBufferPixelShader);

    g_pGAPI->setRasterState(g_pRS_Default);

    g_WVP.world.transpose();
    g_WVP.world = g_worldTransform.getMatrix() * g_pBunnyActor->m_transform.getMatrix();

    memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
    g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

    //g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);
    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);


    g_pBunnyActor->draw();
  }

  ////////////////////////////////////////////////////////////////////////////////////////////   Light Pass
  { 
    g_pGAPI->setVertexShader(g_pDefferredVertexShader);
    g_pGAPI->setPixelShader(g_pDefferredPixelShader);

    g_pGAPI->setInputLayout(g_pDefInputLayout);

    Vector<SPtr<Texture>> rt = {
      g_pGAPI->m_pBackBufferRTV,
      nullptr,
      nullptr
    };

    g_pGAPI->setRenderTargets(3, rt, nullptr);

    g_pGAPI->setRasterState(g_pRS_CullFront);

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


  SAFE_RELEASE(g_pInputLayout);
  SAFE_RELEASE(g_pRS_Default);
  SAFE_RELEASE(g_pRS_Wireframe);
  SAFE_RELEASE(g_pRS_Wireframe_NoCull);
  SAFE_RELEASE(g_pRS_CullFront);
  SAFE_RELEASE(g_pSS_Point);
  SAFE_RELEASE(g_pSS_Linear);
  SAFE_RELEASE(g_pSS_Anisotropic);

  /* SDL will clean up the window/renderer for us. */
  if(g_pWindow) {
    SDL_DestroyWindow(g_pWindow);
    g_pWindow = nullptr;
  }

  GraphicsAPI::shutDown();

  SDL_Quit();
}
