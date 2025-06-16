/**
 * @file    ShaderManager.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    04/06/2025
 */

/**
 * @include
 */
 #include "ShaderManager.h"


ShaderManager& g_shaderManager() {
  return ShaderManager::instance();
}
