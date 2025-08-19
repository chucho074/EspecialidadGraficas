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

#include "Camera.h"
#include "PrerequisiteGraficas.h"
#include "GraphicsAPI.h"
#include "Model.h"
#include "Texture.h"
#include "Transform.h"
#include "SceneGraph.h"
#include "Prop.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "giTime.h"


#include <imgui.h>
#include <imgui_impl_win32.cpp>
#include <imgui_impl_dx11.cpp>

Vector2i g_windowSize = {1280 , 720};

SPtr<SceneGraph> g_pSceneGraph;

SDL_Window* g_pWindow = nullptr;
GraphicsAPI* g_pGAPI = nullptr;
ShaderManager* g_pShaderManager = nullptr;
TextureManager* g_pTextureManager = nullptr;

ShaderRef g_GBufferShaderRef;
ShaderRef g_LightShaderRef;
ShaderRef g_ShadowShaderRef;

MatrixCollection g_WVP;

giTime g_appTime;

SPtr<Camera> g_shadowCamera;

SPtr<Prop> g_pDinoActor;
SPtr<Prop> g_pTerrainActor;

SPtr<Texture> g_rtReflection;
SPtr<Texture> g_dsReflection;

Vector<SPtr<Texture>> gbuffer;
SPtr<Texture> g_dsShadowMap;

Transform g_worldTransform;

void recompileShaders() {
  g_pShaderManager->compileAllShaders();
}

void renderNode(SPtr<Actor> inNode) {
  auto& sg = g_pSceneGraph;

  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  bool tmpOpenNode = false;

  if(0 < inNode->m_children.size()) {  //It has a child
    //tmpOpenNode = ImGui::TreeNodeEx(inNode->m_actor->m_actorName.c_str(), m_rootFlags);
  }
  else {  //It hasn't childs
    ImGui::TreeNodeEx(inNode->m_name.c_str());
  }

  if(ImGui::IsItemClicked()) {
    sg->setSelectedActor(inNode);
  }

  ImGui::TableNextColumn();
  ImGui::TextDisabled("Actor");

  ImGui::TableNextColumn();
  //TODO: Fix the functionality of the checkbox.
  ImGui::Checkbox("", &inNode->isActive);

  if(0 < inNode->m_children.size() && tmpOpenNode) {
    for(auto& nodes : inNode->m_children) {
      //renderNode(reinterpret_cast<Actor>(nodes));
    }
    ImGui::TreePop();
  }
}

void renderUI() {
  auto& gapi = g_graphicsAPI();
  auto& shadMan = g_shaderManager();

  //ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

  if(ImGui::Begin("Menu")) {
    
    if(ImGui::CollapsingHeader("IO")) {
      if(ImGui::Button("Write model bin")) {
        g_pDinoActor->m_model.exportToFile("Models/rex_norm.bin");
      }
      //ImGui::SameLine();
      if(ImGui::Button("Load model from Bin")) {
        
      }
    }
    
    //Cameras
    if(ImGui::CollapsingHeader("CameraSettings")) {
      auto tmpMainCamera = g_pSceneGraph->m_editorCamera;
      
      String tmpX = toString(tmpMainCamera->getPosition().x);
      String tmpY = toString(tmpMainCamera->getPosition().y);
      String tmpZ = toString(tmpMainCamera->getPosition().z);

      ImGui::Text("Camera position");
      ImGui::SameLine();
      ImGui::TextColored({0.91f, 0.07f, 0.14f, 1.f}, tmpX.substr(0, tmpX.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.05f, 0.76f, 0.26f, 1.f}, tmpY.substr(0, tmpY.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.f,   0.48f, 0.8f,  1.f}, tmpZ.substr(0, tmpZ.find(".") + 3).c_str());

      tmpX = toString(Radians(tmpMainCamera->m_YPR.x).getDegrees());
      tmpY = toString(Radians(tmpMainCamera->m_YPR.y).getDegrees());
      tmpZ = toString(Radians(tmpMainCamera->m_YPR.z).getDegrees());

      ImGui::Text("Camera rotation");
      ImGui::SameLine();
      ImGui::TextColored({0.91f, 0.07f, 0.14f, 1.f}, tmpX.substr(0, tmpX.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.05f, 0.76f, 0.26f, 1.f}, tmpY.substr(0, tmpY.find(".") + 3).c_str());
      ImGui::SameLine();
      ImGui::TextColored({0.f,   0.48f, 0.8f,  1.f}, tmpZ.substr(0, tmpZ.find(".") + 3).c_str());

      ImGui::SliderFloat("Camera speed", &tmpMainCamera->m_speed, 1.f, 400.f);
      ImGui::Separator();

      ImGui::SliderFloat("Camera Near", &tmpMainCamera->minZ, 0.001, 0.9);
      ImGui::SliderFloat("Camera Far", &tmpMainCamera->maxZ, 100, 10000);
      ImGui::SliderFloat("Camera FOV", &tmpMainCamera->halfFov, 0.1, 1.54);

    }
    ImGui::Separator(); // Textures
    if(ImGui::CollapsingHeader("Textures")) {
      void* tmpImage;
      
      //Positions
      tmpImage = gbuffer[0]->m_pSRV;
      ImGui::Text("Position Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Normals
      tmpImage = gbuffer[1]->m_pSRV;
      ImGui::Text("Normals Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Albedos
      tmpImage = gbuffer[2]->m_pSRV;
      ImGui::Text("Albedo Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //Shadow map
      tmpImage = g_dsShadowMap->m_pSRV;
      ImGui::Text("Shadow Map");
      ImGui::SameLine();
      ImGui::Image(tmpImage, ImVec2(192, 108));
      ImGui::Separator();
      //
    }
    ImGui::Separator(); // Light
    if(ImGui::CollapsingHeader("Lights")) {
      //ImGui::DragFloat3("Light Direction", &g_WVP.viewPos.x);
      ImGui::ColorEdit3("Light Color", &g_WVP.lightColor.x);
      g_shaderManager().setConstantValues(g_WVP);
      ImGui::Text("Light Direction: %.2f, %.2f, %.2f", 
                  g_WVP.viewPos.x, 
                  g_WVP.viewPos.y, 
                  g_WVP.viewPos.z);

      ////////////////////////////////////////////////////////////////////////////////////////////
      ImGui::Text("Shadow Camera position");
      ImGui::SameLine();
      ImGui::DragFloat3("Position", &g_shadowCamera->m_position.x);

      ImGui::DragFloat("Light intensity", &g_WVP.lightIntensity);
      ImGui::DragFloat("Light Radius", &g_WVP.lightRadius);

      String tmpX = toString(Radians(g_shadowCamera->m_YPR.x).getDegrees());
      String tmpY = toString(Radians(g_shadowCamera->m_YPR.y).getDegrees());
      String tmpZ = toString(Radians(g_shadowCamera->m_YPR.z).getDegrees());

      ImGui::Text("Shadow Camera rotation");
      ImGui::SameLine();
      ImGui::DragFloat3("YawPitchRoll", &g_shadowCamera->m_YPR.x);
    }
    ImGui::Separator(); // Scene Graph
    if(ImGui::CollapsingHeader("Scene Graph")) {
      ImGui::Text("Scene ID: %s", toString(g_pSceneGraph->m_sceneID.getUID()).c_str());
      ImGui::Text("Scene Name: %s", g_pSceneGraph->m_sceneName.c_str());
      ImGui::Text("Scene Graph Actors: %d", g_pSceneGraph->m_numActors);

      //Hierarchy list
      if(ImGui::BeginTable("Hierarchy List", 3, ImGuiTableFlags_Resizable | 
                                                ImGuiTableFlags_NoBordersInBody |
                                                ImGuiTableFlags_BordersV)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Active", ImGuiTableColumnFlags_NoHide);
        ImGui::TableHeadersRow();

        ImGui::SetNextItemOpen(true, ImGuiCond_Once);

        //renderNode();

        ImGui::EndTable();
      }
      /*if(ImGui::Button("Spawn Prop")) {
        g_pSceneGraph->spawnActor<Prop>(g_pSceneGraph->getRoot(), 
                                        Vector3(0, 0, 0), 
                                        Vector3(1, 1, 1));
      }
      if(ImGui::Button("Spawn Light")) {
        g_pSceneGraph->spawnActor<Light>(g_pSceneGraph->getRoot(), 
                                         Vector3(0, 0, 0), 
                                         Vector3(1, 1, 1));
      }*/

    }


    ImGui::Separator(); // Delta Time
    {
      ImGui::Text("Delta Time: %.3f ms/frame", g_appTime.getTime());
      ImGui::Text("FPS: %.1f", 1.f / g_appTime.getTime());

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
    ImGui::Separator(); // Delta Time
    {
      ImGui::Text("Delta Time: %.6f ms", g_appTime.getTime());

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }
  }
  ImGui::End();
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
    //Initialize the ShaderManager
    ShaderManager::startUp();
    ShaderManager* shMan = new ShaderManager();
    g_shaderManager().setObject(shMan);
    g_pShaderManager = &g_shaderManager();

    TextureManager::startUp();
    TextureManager* texMan = new TextureManager();
    g_textureManager().setObject(texMan);
    g_pTextureManager = &g_textureManager();
    g_pTextureManager->init();

    recompileShaders();
  }
  
  // Create the shaders

  {
    g_ShadowShaderRef = g_pShaderManager->createShaderProgram("Shaders/GBuffer.hlsl",
                                                               "shadow_map_vertex_main",
                                                               "shadow_map_pixel_main");

    g_shaderManager().setSamplerToShader(g_ShadowShaderRef, SAMPLER_USAGE::kAll);
    g_shaderManager().setRasterToShader(g_ShadowShaderRef, RASTER_USAGE::kDefault);

    g_GBufferShaderRef = g_pShaderManager->createShaderProgram("Shaders/GBuffer.hlsl",
                                                               "gbuffer_vertex_main",
                                                               "gbuffer_pixel_main");

    g_shaderManager().setSamplerToShader(g_GBufferShaderRef, SAMPLER_USAGE::kAll);
    g_shaderManager().setRasterToShader(g_GBufferShaderRef, RASTER_USAGE::kDefault);
    
    g_shaderManager().setDefaultShader(g_GBufferShaderRef);

    g_LightShaderRef = g_pShaderManager->createShaderProgram("Shaders/lightShader.hlsl",
                                                             "vertex_main",
                                                             "pixel_main");

    g_shaderManager().setSamplerToShader(g_LightShaderRef, SAMPLER_USAGE::kAll);
    g_shaderManager().setRasterToShader(g_LightShaderRef, RASTER_USAGE::kCullFront);
  }

  //Create the window and renderer
  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
   
  // Create systems & Initialize Values

  {
    g_pSceneGraph = make_shared<SceneGraph>();
    g_pSceneGraph->init();
    auto& cameraRef = g_pSceneGraph->m_editorCamera;

    
    //Set world info
    g_WVP.world.identity();
    g_WVP.view = cameraRef->getViewMatrix();
    g_WVP.projection = cameraRef->getProjectionMatrix();

  g_WVP.viewPos = cameraRef->getPosition();
  g_WVP.time = 1.f;

    g_WVP.lightIntensity = 1.f;
    g_WVP.lightRadius = 250.f;

    g_shadowCamera = make_shared<Camera>();

    g_shadowCamera->setLookAt(Vector3(-10, 5, 0), Vector3(0, 0, 0), Vector3(0, 1, 0));
    //g_shadowCamera->setLookAt(Vector3(-65, 35, 50), Vector3(0, 0, 0), Vector3(0, 1, 0));
    //g_shadowCamera->setOrthographic(-0.75f, 0.75f, -0.75f, 0.75f, 0.1f, 500.f);  //El bueno 
    g_shadowCamera->setOrthographic(-5.f,   5.f,   -5.f,   5.f,   0.1f, 500.f);  //Testing
    //g_shadowCamera->setPerspectiveHalf(3.1415926353f / 4.f, g_windowSize, 0.1f, 1000.f);

    g_WVP.lightView = g_shadowCamera->getViewMatrix();

    g_WVP.world.transpose();
    g_WVP.view.transpose();
    g_WVP.projection.transpose();

    //g_WVP.lightProjection.OrthographicLH(-5.f, 5.f, -5.f, 5.f, 0.1f, 500.f);
    g_WVP.lightProjection = g_shadowCamera->getOrthoMatrix();
    g_WVP.lightView.transpose();
    g_WVP.lightColor = Vector3(1.f, 1.f, 1.f);
  }


  g_WVP.lightColor = {1.f, 1.f, 1.f};
  g_WVP.lightIntensity = 1.f;
  g_WVP.lightRadius = 250.f;
  g_WVP.lightPosition = g_shadowCamera->getPosition();


  //Load models and textures
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Dino
  g_pDinoActor = static_pointer_cast<Prop>(g_pSceneGraph->spawnActor<Prop>(g_pSceneGraph->getRoot(), 
                                                                           Vector3(0, 0, 0), 
                                                                           Vector3(1, 1, 1)));

  //Rex model
  //if(!g_pDinoActor->m_model.loadFromBin("Models/rex_norm.bin")) {
  //if(!g_pDinoActor->m_model.loadFromBin("Models/BistroExt.bin")) {
  //if(!g_pDinoActor->m_model.loadFromBin("Models/R8_chico.bin")) {
  //if(!g_pDinoActor->m_model.loadFromBin("Models/bunny.bin")) {
  
  //if(!g_pDinoActor->m_model.loadFromFile("Models/BistroExt.obj")) {
  
  
  if(!g_pDinoActor->m_model.loadFromFile("Models/Rex/Rex_mat.obj")) {
  //if(!g_pDinoActor->m_model.loadFromFile("Models/bistro/Exterior/exterior.obj")) {
  //if(!g_pDinoActor->m_model.loadFromFile("Models/BistroExt.obj")) {
  //if(!g_pDinoActor->m_model.loadFromFile("Models/R8_chico.obj")) {
  //if(!g_pDinoActor->m_model.loadFromFile("Models/bunny.obj")) {
    __debugbreak();
    return SDL_APP_FAILURE;
  }

  /*g_pDinoActor->m_material.setAlbedo("Models/Rex_C.bmp");
  g_pDinoActor->m_material.setNormalTexture("Models/Rex_N.bmp");
  g_pDinoActor->m_material.setRoughnessTexture("Models/Rex_R.bmp");
  g_pDinoActor->m_material.setMetalicTexture("Models/Rex_M.bmp");
  g_pDinoActor->m_material.setShaderRef(g_GBufferShaderRef);*/



  ////////////////////////////////////////////////////////////////////////////////////////////  Terrain
  
  g_pTerrainActor = static_pointer_cast<Prop>(g_pSceneGraph->spawnActor<Prop>(g_pSceneGraph->getRoot(), 
                                                                              Vector3(0, 0, 0), 
                                                                              //Vector3(1.f, 1.f, 1.f)));
                                                                              Vector3(10.f, 10.f, 10.f)));

  //Disc model
  //if(!g_pTerrainActor->m_model.loadFromFile("Models/disc.obj")) {
  if(!g_pTerrainActor->m_model.loadFromFile("Models/Plane.obj")) {
    __debugbreak();
    return SDL_APP_FAILURE;
  }

  //g_pTerrainActor->m_material.setAlbedo("Models/Terrain.bmp");
  //g_pTerrainActor->m_material.setShaderRef(g_GBufferShaderRef);


  g_rtReflection = make_shared<Texture>();
  g_dsReflection = make_shared<Texture>();
  g_dsShadowMap = make_shared<Texture>();


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

  g_dsShadowMap->m_pTexture = g_pGAPI->createTexture(g_windowSize.x,
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_D32_FLOAT,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
                                                     0,
                                                     1,
                                                     &g_dsShadowMap->m_pSRV,
                                                     &g_dsShadowMap->m_pRTV,
                                                     &g_dsShadowMap->m_pDSV,
                                                     &g_dsShadowMap->m_pDSV_RO);

  ////////////////////////////////////////////////////////////////////////////////////////////  ImGui

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  io.DisplaySize = ImVec2(g_windowSize.x, g_windowSize.y);
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGuiStyle& style = ImGui::GetStyle();
  if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.WindowRounding = 0.0f;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
  }

  ImGui::StyleColorsDark();

  ImGui_ImplWin32_Init(pHandle);
  ImGui_ImplDX11_Init((ID3D11Device*)g_pGAPI->getDevice(),
                      (ID3D11DeviceContext*)g_pGAPI->getDeviceContext());

  //Change font path to engine settings
  io.Fonts->AddFontFromFileTTF("Fonts/Inter-Regular.ttf", 15.0f);

  ////////////////////////////////////////////////////////////////////////////////////////////

  g_appTime.startTimer();

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  auto& cameraRef = g_pSceneGraph->m_editorCamera;

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
      cameraRef->m_front = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_W) {
      cameraRef->m_front = false;
    }
  }
  //Get backard input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_S) {
      cameraRef->m_back = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_S) {
      cameraRef->m_back = false;
    }
  }
  //Get left input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_A) {
      cameraRef->m_left = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_A) {
      cameraRef->m_left = false;
    }
  }
  //Get right input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_D) {
      cameraRef->m_right = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_D) {
      cameraRef->m_right = false;
    }
  }
  //Get up input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_E) {
      cameraRef->m_up = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_E) {
      cameraRef->m_up = false;
    }
  }
  //Get down input
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Q) {
      cameraRef->m_down = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Q) {
      cameraRef->m_down = false;
    }
  }

  //Rotate
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_Z) {
      cameraRef->m_YawNeg = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_Z) {
      cameraRef->m_YawNeg = false;
    }
  }
  if(event->type == SDL_EVENT_KEY_DOWN) {
    if(event->key.key == SDLK_C) {
      cameraRef->m_YawPos = true;
    }
  }
  else if(event->type == SDL_EVENT_KEY_UP) {
    if(event->key.key == SDLK_C) {
      cameraRef->m_YawPos = false;
    }
  }

  ImGuiIO& io = ImGui::GetIO();
  //Mouse Pressed case
  {
    int32 button = -1;
    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      button = event->button.button == SDL_BUTTON_LEFT ? 0 : button;

      button = event->button.button == SDL_BUTTON_RIGHT ? 1 : button;

      button = event->button.button == SDL_BUTTON_MIDDLE ? 2 : button;

      button = event->button.button == SDL_BUTTON_X1 ? 3 : button;

      button = event->button.button == SDL_BUTTON_X2 ? 4 : button;
      if(button > -1) {
        io.MouseDown[button] = true;
      }
    }
  }
  //Mouse released case
  {
    int32 button = -1;
    if(event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
      button = event->button.button == SDL_BUTTON_LEFT ? 0 : button;

      button = event->button.button == SDL_BUTTON_RIGHT ? 1 : button;

      button = event->button.button == SDL_BUTTON_MIDDLE ? 2 : button;

      button = event->button.button == SDL_BUTTON_X1 ? 3 : button;

      button = event->button.button == SDL_BUTTON_X2 ? 4 : button;
      if(button > -1) {
        io.MouseDown[button] = false;
      }
    }
  }

  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {
  auto& cameraRef = g_pSceneGraph->m_editorCamera;
  
  g_appTime.update();
  cameraRef->move(g_appTime.getTime());
  g_pSceneGraph->update(g_appTime.getTime());

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
    g_pGAPI->clearDSV(g_dsShadowMap);

    for(auto tex : gbuffer) {
      g_pGAPI->clearRTV(tex, blackClearColor);
    }
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Values for cbuffer
  static float rotationAngle = 0.f;
  static float tempo = 0.f;
  tempo += 0.1f;
  g_WVP.time = tempo;

  g_WVP.world.identity();
  g_WVP.view = cameraRef->getViewMatrix();
  g_WVP.projection = cameraRef->getProjectionMatrix();
  g_WVP.view.transpose();
  g_WVP.projection.transpose();
  
  g_WVP.lightView = g_shadowCamera->getViewMatrix();
  g_WVP.lightView.transpose();
  g_pShaderManager->setConstantValues(g_WVP);

  ////////////////////////////////////////////////////////////////////////////////////////////  Shadow Pass
  
  {

    g_pShaderManager->setConstantValues(g_WVP);
    g_pShaderManager->setDataToShader(g_ShadowShaderRef);

    Vector<SPtr<Texture>> rt = {
      nullptr,
      nullptr,
      nullptr
    };

    g_pGAPI->setRenderTargets(3, rt, g_dsShadowMap);

    //g_pSceneGraph->draw(false);
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Draw in GBuffer

  {
    g_pGAPI->setRenderTargets(gbuffer.size(), gbuffer, g_pGAPI->m_pBackBufferDSV);

    g_pSceneGraph->draw(true);
  }
  
  ////////////////////////////////////////////////////////////////////////////////////////////  Light Pass
  
  { 
    
    g_pShaderManager->setConstantValues(g_WVP);
    g_pShaderManager->setDataToShader(g_LightShaderRef);
    g_WVP.lightPosition = g_shadowCamera->getPosition();
    //g_WVP.viewPos = g_shadowCamera->getCameraDir();
    g_WVP.viewPos = cameraRef->getPosition();
    g_pShaderManager->setConstantValues(g_WVP);

    Vector<SPtr<Texture>> rt = {
      g_pGAPI->m_pBackBufferRTV,
      nullptr,
      nullptr
    };

    g_pGAPI->setRenderTargets(3, rt, nullptr);

    g_pGAPI->setShaderResource(0, gbuffer[0]);
    g_pGAPI->setShaderResource(1, gbuffer[1]);
    g_pGAPI->setShaderResource(2, gbuffer[2]);
    g_pGAPI->setShaderResource(3, nullptr);
    g_pGAPI->setShaderResource(4, g_dsShadowMap);

    g_pGAPI->m_pDeviceContext->Draw(3, 0);

    g_pGAPI->setShaderResource(0, nullptr);
    g_pGAPI->setShaderResource(1, nullptr);
    g_pGAPI->setShaderResource(2, nullptr);
    g_pGAPI->setShaderResource(3, nullptr);
    g_pGAPI->setShaderResource(4, nullptr);

  }

  //////////////////////////////////////////////////////////////////////////////////////////////  ImGui

  {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGuiIO& io = ImGui::GetIO();

    //ImGui::ShowDemoWindow();

    renderUI();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  Present

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

  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  GraphicsAPI::shutDown();

  SDL_Quit();
}