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

#include "GraphicsAPI.h"

struct SimpleVertex {
  Vector3 position;
  Vector3 color;
};


SDL_Window* g_pWindow = nullptr;
UPtr<GraphicsAPI> g_pGPAI;
UPtr<VertexShader> g_pVertexShader;
UPtr<PixelShader> g_pPixelShader;
ID3D11InputLayout* g_pInputLayout = nullptr;
UPtr<GraphicsBuffers> g_pVertexBuffer;
UPtr<GraphicsBuffers> g_pIndexBuffer;

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
                               640, 
                               480, 
                               SDL_WINDOW_RESIZABLE);

  auto pHandle = SDL_GetPointerProperty(SDL_GetWindowProperties(g_pWindow), 
                                        SDL_PROP_WINDOW_WIN32_HWND_POINTER,
                                        nullptr);
  if(pHandle) {
    g_pGPAI = make_unique<GraphicsAPI>(pHandle);

    if (!g_pGPAI) {
      return SDL_APP_FAILURE;
    }

    g_pVertexShader = g_pGPAI->createVertexShaderFromFile("Shaders/basicVertexShader.hlsl", 
                                                          "vertex_main");
    if(!g_pVertexShader) {
      return SDL_APP_FAILURE;
    }

    g_pPixelShader = g_pGPAI->createPixelShaderFromFile("Shaders/basicVertexShader.hlsl", 
                                                        "pixel_main");
    if (!g_pPixelShader) {
      return SDL_APP_FAILURE;
    }
  }

  Vector<D3D11_INPUT_ELEMENT_DESC> inputElementDescs = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
  };

  g_pInputLayout = g_pGPAI->createInputLayout(inputElementDescs, g_pVertexShader);
  if (!g_pInputLayout) {
    return SDL_APP_FAILURE;
  }

  SimpleVertex vertex[] = {
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


  unsigned short indices[] = {
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

  Vector<char> tmpVertexData;
  tmpVertexData.resize(sizeof(vertex));
  memcpy(tmpVertexData.data(), vertex, sizeof(vertex));
  g_pVertexBuffer = g_pGPAI->createVertexBuffer(tmpVertexData);

  if(!g_pVertexBuffer) {
    return SDL_APP_FAILURE;
  }

  Vector<char> tmpIndexData;
  tmpIndexData.resize(sizeof(indices));
  memcpy(tmpIndexData.data(), indices, sizeof(indices));
  g_pIndexBuffer = g_pGPAI->createIndexBuffer(tmpIndexData);

  if (!g_pVertexBuffer) {
    return SDL_APP_FAILURE;
  }


  if(!g_pWindow) {
    SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }


  return SDL_APP_CONTINUE;
}


/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult
SDL_AppEvent(void* appstate, SDL_Event* event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult
SDL_AppIterate(void* appstate) {

  /* put the newly-cleared rendering on the screen. */
  //SDL_RenderPresent(g_renderer);

  //Rellenar el input assembly
  //IA > Input Assambly
  //OM > Output Merger
  D3D11_VIEWPORT vp;
  vp.Width = 640;
  vp.Height = 480;
  vp.MinDepth = 0.f;
  vp.MaxDepth = 1.f;
  vp.TopLeftX = 0; 
  vp.TopLeftY = 0;

  g_pGPAI->m_pDeviceContext->OMSetRenderTargets(1, 
                                                &g_pGPAI->m_pBackBufferRTV, 
                                                g_pGPAI->m_pBackBufferDSV);
  
  float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
  g_pGPAI->m_pDeviceContext->ClearRenderTargetView(g_pGPAI->m_pBackBufferRTV, 
                                                   clearColor);
  g_pGPAI->m_pDeviceContext->ClearDepthStencilView(g_pGPAI->m_pBackBufferDSV, 
                                                   D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                                   1.f, 
                                                   0);

  g_pGPAI->m_pDeviceContext->VSSetShader(g_pVertexShader->m_pVertexShader, nullptr, 0);
  g_pGPAI->m_pDeviceContext->PSSetShader(g_pPixelShader->m_pPixelShader, nullptr, 0);

  g_pGPAI->m_pDeviceContext->IASetInputLayout(g_pInputLayout);
  g_pGPAI->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  UINT stride = sizeof(SimpleVertex);
  UINT offset = 0;
  g_pGPAI->m_pDeviceContext->IASetVertexBuffers(0, 
                                                1,
                                                &g_pVertexBuffer->m_pBuffer,
                                                &stride,
                                                &offset);


  g_pGPAI->m_pDeviceContext->IASetIndexBuffer(g_pIndexBuffer->m_pBuffer,
                                              DXGI_FORMAT_R16_UINT,
                                              0);
  //g_pGPAI->m_pDeviceContext->Draw(3, 0);
  g_pGPAI->m_pDeviceContext->DrawIndexed(36, 0, 0);
  g_pGPAI->m_pSwapChain->Present(0, 0);

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
  SDL_Quit();
}
