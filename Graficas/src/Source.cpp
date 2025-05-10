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
UPtr<GraphicsAPI> g_pGAPI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;
UPtr<PixelShader> g_pPixelShader_Reflect;
ID3D11InputLayout* g_pInputLayout = nullptr;
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

Camera g_Camera;

Model g_cubeModel;

SPtr<Prop> g_pDinoActor;
SPtr<Prop> g_pCarActor;
SPtr<Prop> g_pTerrainActor;

Texture g_rtReflection;
Texture g_dsReflection;

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
    g_pGAPI = make_unique<GraphicsAPI>(pHandle);

    if (!g_pGAPI) {
      return SDL_APP_FAILURE;
    }

    recompileShaders();
  }

  Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };
  
  g_pInputLayout = g_pGAPI->createInputLayout(inputElementDescs, g_pVertexShader);
  
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
  g_Camera.setLookAt(Vector3(5, -5, -5), Vector3(0, 0, 0), Vector3(0, 1, 0));
  g_Camera.setPerspective(3.1415926353f/4.f, g_windowSize, 0.1f, 100.f);
  
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
  
  /////////////////////////////////////////////////////////////////////////////
  
  //Cube model
  Vector<SimpleVertex> vertex = {
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 1.0f), -0.f },

    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(0, 1, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(1, 0, 0)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(1, 0, 0)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f, -1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 1.0f), -0.f },
    
    { Vector3(-1.0f, -1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(0.0f, 0.0f), -0.f },
    { Vector3( 1.0f, -1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 0.0f), -0.f },
    { Vector3( 1.0f,  1.0f,  1.0f), Vector3(0, 0, 1)  }, //Vector2(1.0f, 1.0f), -0.f },
    { Vector3(-1.0f,  1.0f,  1.0f), Vector3(0, 0, 1)  }  //Vector2(0.0f, 1.0f), -0.f },

  };

  Vector<uint32> indices = {
      3,1,0,
      2,1,3,

      6,4,5,
      7,4,6,

      11,9,8,
      10,9,11,

      14,12,13,
      15,12,14,

      19,17,16,
      18,17,19,

      22,20,21,
      23,20,22
  };

  if(!g_cubeModel.loadFromMem(vertex, indices, g_pGAPI)) {
    return SDL_APP_FAILURE;
  }
  /////////////////////////////////////////////////////////////////////////////
  
  //Car model
  g_pCarActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(), Vector3(0, 0, 0), Vector3(3.f,3.f,3.f)));

  //if(!g_carModel.loadFromFile("Models/audi.obj", g_pGAPI)) {
  //if(!g_pCarActor->m_model.loadFromFile("Models/rex.obj", g_pGAPI)) { 
  if(!g_pCarActor->m_model.loadFromFile("Models/rex_norm.obj", g_pGAPI)) { 
    return SDL_APP_FAILURE;
  }

  Image carImage;
  carImage.decode("Models/Untitled.bmp");
  g_pCarActor->m_texture.createFromImage(carImage, g_pGAPI);

  /////////////////////////////////////////////////////////////////////////////
  

  g_pDinoActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(), Vector3(0,0,0)));

  //Rex model
  if(!g_pDinoActor->m_model.loadFromFile("Models/rex_norm.obj", g_pGAPI)) {
    __debugbreak();
    return SDL_APP_FAILURE;
  }

  Image srcImage;
  srcImage.decode("Models/Rex_C.bmp");
  g_pDinoActor->m_texture.createFromImage(srcImage, g_pGAPI);

  /////////////////////////////////////////////////////////////////////////////
  
  g_pTerrainActor = static_pointer_cast<Prop>(g_sceneGraph->spawnActor<Prop>(g_sceneGraph->getRoot(), Vector3(0, 0, 0), Vector3(0.08f, 0.08f, 0.08f)));

  //Disc model
  if(!g_pTerrainActor->m_model.loadFromFile("Models/disc.obj", g_pGAPI)) {
    return SDL_APP_FAILURE;
  }

  Image terrainImage;
  terrainImage.decode("Models/Terrain.bmp");
  g_pTerrainActor->m_texture.createFromImage(terrainImage, g_pGAPI);

  //Reflection textures
  g_rtReflection.m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_B8G8R8A8_UNORM,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 
                                                     0,
                                                     1,
                                                     &g_rtReflection.m_pSRV,
                                                     &g_rtReflection.m_pRTV);

  g_dsReflection.m_pTexture = g_pGAPI->createTexture(g_windowSize.x, 
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_D24_UNORM_S8_UINT,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_DEPTH_STENCIL, 
                                                     0,
                                                     1,
                                                     nullptr,
                                                     nullptr,
                                                     &g_dsReflection.m_pDSV);

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

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {

  g_sceneGraph->update(0.f);

  //Rellenar el input assembly
  //IA > Input Assambly
  //OM > Output Merger
  D3D11_VIEWPORT vp;
  vp.Width = g_windowSize.x;
  vp.Height = g_windowSize.y;
  vp.MinDepth = 0.f;
  vp.MaxDepth = 1.f;
  vp.TopLeftX = 0; 
  vp.TopLeftY = 0;

  g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, 
                            g_pGAPI->m_pBackBufferDSV);
  
  FloatColor clearColor = { 0.5f, 0.5f, 1.0f, 1.0f };
  FloatColor blackClearColor = { 0.0f, 0.f, 0.0f, 1.0f };
  g_rtReflection.clearTexture(blackClearColor.toArray(), g_pGAPI);
  g_dsReflection.clearTexture(blackClearColor.toArray(), g_pGAPI);

  g_pGAPI->clearRTV(g_pGAPI->m_pBackBufferRTV, clearColor);

  g_pGAPI->clearDSV(g_pGAPI->m_pBackBufferDSV);

  g_pGAPI->setVertexShader(g_pVertexShader);
  g_pGAPI->setPixelShader(g_pPixelShader);

  g_pGAPI->setInputLayout(g_pInputLayout);
  g_pGAPI->setTopology(g_pDinoActor->m_model.m_meshes[0].topology);
  
  g_cubeModel.setBuffers(g_pGAPI);
  
  static float rotationAngle = 0.f;
  rotationAngle += 0.01f;

  Matrix4 translation3; //000
  translation3.identity();
  translation3.Translate({0, 0, 0});

  //g_worldTransform.setRotation({0, rotationAngle, 0});

  static float tempo = 0.f;
  tempo += 0.0001f;
  
  g_WVP.time = tempo;

  Vector<char> matrix_data;
  matrix_data.resize(sizeof(g_WVP));

  g_pGAPI->setConstantBuffer(0, g_pCB_WVP);

  //g_pGAPI->setRasterState(g_pRS_Default);
  g_pGAPI->setRasterState(g_pRS_Wireframe);

  //Set the samplers
  g_pGAPI->setSamplers(0, g_pSS_Point);
  g_pGAPI->setSamplers(0, g_pSS_Linear);
  g_pGAPI->setSamplers(0, g_pSS_Anisotropic);

  ////////////////////////////////////////////////////////////////////////////////////////////    Car
  
  g_WVP.world = g_worldTransform.getMatrix() * g_pCarActor->m_transform.getMatrix();

  g_WVP.world.transpose();
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);
  
  //g_pCarActor->draw(g_pGAPI);

  ////////////////////////////////////////////////////////////////////////////////////////////    Rex
  
  g_pGAPI->setRasterState(g_pRS_Default);

  g_WVP.world.transpose();
  g_WVP.world = g_worldTransform.getMatrix() * g_pDinoActor->m_transform.getMatrix();
  
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);
  
  g_pDinoActor->draw(g_pGAPI);
  //g_sceneGraph->draw(g_pGAPI);

  ////////////////////////////////////////////////////////////////////////////////////////////  Reflection
  
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
  
  //g_pDinoActor->draw(g_pGAPI);

  ////////////////////////////////////////////////////////////////////////////////////////////  Floor

  g_WVP.world = g_worldTransform.getMatrix() * g_pTerrainActor->m_transform.getMatrix();

  g_WVP.world.transpose();
  memcpy(matrix_data.data(), &g_WVP, sizeof(g_WVP));
  g_pGAPI->writeToBuffer(g_pCB_WVP, matrix_data);

  g_pGAPI->setRasterState(g_pRS_Default);

  //g_TerrainModel.setBuffers(g_pGAPI);

  g_pGAPI->setRenderTargets(g_pGAPI->m_pBackBufferRTV, g_pGAPI->m_pBackBufferDSV);

  g_pGAPI->setPixelShader(g_pPixelShader_Reflect);

  //g_pGAPI->setShaderResource(0, g_TerrainTexture);
  g_pGAPI->setShaderResource(1, g_rtReflection);


  //g_TerrainModel.draw(g_pGAPI);


  g_pGAPI->m_pDeviceContext->PSSetShader(g_pPixelShader->m_pPixelShader, nullptr, 0);
  ////////////////////////////////////////////////////////////////////////////////////////////
  
  g_pGAPI->m_pSwapChain->Present(0, 0);

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
  SDL_Quit();
}
