/**
 * @file    Configs.h
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
#include "Module.h"


class Configs : public Module<Configs> {
 public:
  Configs() = default;
  ~Configs() = default;
  
  //Shaders info
  static Path s_gbufferShaderPath;
  static String s_gbufferEntryPointVS;
  static String s_gbufferEntryPointPS;

  static Path s_lightShaderPath;
  static String s_lightEntryPointVS;
  static String s_lightEntryPointPS;

  static Path s_shadowShaderPath;
  static String s_shadowEntryPointVS;
  static String s_shadowEntryPointPS;

  //Window info
  static Vector2i s_windowSize;
  static bool s_vsync;
  static bool s_fullScreen;
  static float s_aspectRatio;
  static String s_windowTitle;

  //Renderer
  static Vector2i s_renderTextureSize;
  static Vector2i s_shadowTextureSize;

  //App info
  static Path s_defaultFontPath;
};

Configs&
g_configs();