/**
 * @file    Renderer.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    09/06/2025
 */

/**
 * @include
 */
#pragma once
#include "Module.h"

class Transform;
class Vector2i;

class Renderer : public Module<Renderer> {
 public:
  Renderer() = default;
  ~Renderer() = default;

  void
  create();

  void
  update(float inDelta);

  void
  render();

  void
  destroy();


  void
  setTransform(Transform inTransform);

  void
  resize(Vector2i inScreenSize);
  
  void
  updateShaders();


 protected:
	
};