/**
 * @file    Prop.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    05/5/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Actor.h"
#include "Model.h"
#include "Texture.h"
#include "GraphicsAPI.h"


class Prop : public Actor {
 public:
  Prop() = default;
  ~Prop() = default;

  virtual void
  init(Vector3 inPos, Vector3 inScale = {1.f, 1.f, 1.f}, Vector3 inRotation = Vector3::ZERO) override {
    Actor::init(inPos, inScale, inRotation);

  }

  virtual void
  update(float inDT) override {
    Actor::update(inDT);

  }

  virtual void
  draw(const UPtr<GraphicsAPI>& inGAPI) override {

    m_model.setBuffers(inGAPI);
    inGAPI->setShaderResource(0, m_texture);

    m_model.draw(inGAPI);
  }



  Model m_model;
  Texture m_texture;
};