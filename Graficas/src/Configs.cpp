/**
 * @file    Configs.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    29/09/2025
 * @brief   .
 */

/**
 * @include
 */
#include "Configs.h"

//Shaders info
Path Configs::s_gbufferShaderPath     = "Shaders/GBuffer.hlsl";
String Configs::s_gbufferEntryPointVS = "gbuffer_vertex_main";
String Configs::s_gbufferEntryPointPS = "gbuffer_pixel_main";

Path Configs::s_lightShaderPath     = "Shaders/lightShader.hlsl";
String Configs::s_lightEntryPointVS = "vertex_main";
String Configs::s_lightEntryPointPS = "pixel_main";

Path Configs::s_shadowShaderPath     = "Shaders/GBuffer.hlsl";
String Configs::s_shadowEntryPointVS = "shadow_map_vertex_main";
String Configs::s_shadowEntryPointPS = "shadow_map_pixel_main";

//Window info
Vector2i Configs::s_windowSize = {1280 , 720};
bool Configs::s_vsync = true;
bool Configs::s_fullScreen = false;
float Configs::s_aspectRatio = (float)s_windowSize.x / (float)s_windowSize.y;
String Configs::s_windowTitle = "Graficas Basicas";

//Renderer
Vector2i Configs::s_renderTextureSize = {1280 , 720};
Vector2i Configs::s_shadowTextureSize = {1280 , 720};

//App info
Path Configs::s_defaultFontPath = "Fonts/Inter-Regular.ttf";




Configs&
g_configs() {
  return Configs::instance();
}