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
#include "Texture.h"

#include "App.h"
#include "Configs.h"

#include <imgui.h>


Vector2i g_windowSize = {1280 , 720};

SDL_Window* g_pWindow = nullptr;

Vector2 g_viewportSize = Vector2::ZERO;
Vector2 g_prevViewportSize = Vector2::ZERO;

void resizeTextures() {
  
  SDL_GetWindowSize(g_pWindow, &g_windowSize.x, &g_windowSize.y);

  g_prevViewportSize = g_viewportSize;

  //Unbind shader resources
  auto& gapi = g_graphicsAPI();
  for(int32 i = 0; i < 5; ++i) {
    gapi.clearSRV(i);
  }

  Vector2i newSize;
  newSize = g_windowSize;
  /*if(g_prevViewportSize == Vector2::ZERO) {
    newSize = g_windowSize;
    g_prevViewportSize = Vector2(g_windowSize.x, g_windowSize.y);
  }
  else {
    newSize = {(int32)g_prevViewportSize.x,
               (int32)g_prevViewportSize.y};
  }*/

  if(g_app().gbuffer.size() > 0) {
    g_app().gbuffer.clear();
    gapi.resizeBackBuffer(Vector2(g_windowSize.x, g_windowSize.y));
  }

  ////////////////////////////////////////////////////////////////////////////////////////////  GBuffer
  g_app().gbuffer.resize(3);
  for(int i = 0; i < g_app().gbuffer.size(); ++i) {
    g_app().gbuffer[i] = make_shared<Texture>();
  }

  //Pos
  g_app().gbuffer[0]->m_pTexture = g_graphicsAPI().createTexture(newSize.x,
                                                  newSize.y,
                                                  DXGI_FORMAT_R32G32B32A32_FLOAT,
                                                  //DXGI_FORMAT_R16G16B16A16_FLOAT,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &g_app().gbuffer[0]->m_pSRV,
                                                  &g_app().gbuffer[0]->m_pRTV);
  //Normals
  g_app().gbuffer[1]->m_pTexture = g_graphicsAPI().createTexture(newSize.x,
                                                  newSize.y,
                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &g_app().gbuffer[1]->m_pSRV,
                                                  &g_app().gbuffer[1]->m_pRTV);
  //Color
  g_app().gbuffer[2]->m_pTexture = g_graphicsAPI().createTexture(newSize.x,
                                                  newSize.y,
                                                  DXGI_FORMAT_R8G8B8A8_UNORM,
                                                  D3D11_USAGE_DEFAULT,
                                                  D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                  0,
                                                  1,
                                                  &g_app().gbuffer[2]->m_pSRV,
                                                  &g_app().gbuffer[2]->m_pRTV);

  ////////////////////////////////////////////////////////////////////////////////////////////  Render Pass RT

  g_app().m_renderPassRT = make_shared<Texture>();
  g_app().m_renderPassRT->m_pTexture = g_graphicsAPI().createTexture(newSize.x,
                                                      newSize.y,
                                                      DXGI_FORMAT_B8G8R8A8_UNORM,
                                                      D3D11_USAGE_DEFAULT,
                                                      D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
                                                      0,
                                                      1,
                                                      &g_app().m_renderPassRT->m_pSRV,
                                                      &g_app().m_renderPassRT->m_pRTV);


  ////////////////////////////////////////////////////////////////////////////////////////////  Shadow Map Texture

  g_app().m_dsShadowMap->m_pTexture = g_graphicsAPI().createTexture(g_windowSize.x,
                                                     g_windowSize.y,
                                                     DXGI_FORMAT_D32_FLOAT,
                                                     D3D11_USAGE_DEFAULT,
                                                     D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE,
                                                     0,
                                                     1,
                                                     &g_app().m_dsShadowMap->m_pSRV,
                                                     &g_app().m_dsShadowMap->m_pRTV,
                                                     &g_app().m_dsShadowMap->m_pDSV,
                                                     &g_app().m_dsShadowMap->m_pDSV_RO);

}

void recompileShaders() {
  g_shaderManager().compileAllShaders();
}

/* This function runs once at startup. */
SDL_AppResult
SDL_AppInit(void** appstate, int argc, char* argv[]) {
  
  App::startUp();
  App* app = new App();
  g_app().setObject(app);
  

  //Initialize sdl
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Log("SDL Initialized!");

  //Create sdl window
  g_pWindow = SDL_CreateWindow(g_configs().s_windowTitle.c_str(), 
                               g_configs().s_windowSize.x, 
                               g_configs().s_windowSize.y, 
                               SDL_WINDOW_RESIZABLE);
                               
  //Create the window and renderer
  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_Log("Window created!");


  auto pHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(g_pWindow), 
                                        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                        nullptr);
  //Initialize the application
  if(!pHandle) {
    SDL_Log("Couldn't get window handle: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  g_app().init(pHandle);

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

  g_app().m_appTime.startTimer();

  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  auto& cameraRef = g_app().m_pSceneGraph->m_editorCamera;

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
  
  g_app().update(0.f);

  g_app().render();

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

  g_app().destroy();

  SDL_Quit();
}