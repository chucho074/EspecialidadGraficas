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

  void
  init(Vector3 inPos, 
       Vector3 inScale = Vector3::UNIT, 
       Vector3 inRotation = Vector3::ZERO) override {
    Actor::init(inPos, inScale, inRotation);

  }

  void
  update(float inDT) override {
    Actor::update(inDT);

  }

  void
  draw(const UPtr<GraphicsAPI>& inGAPI) override {

    m_model.setBuffers(inGAPI);
    inGAPI->setShaderResource(0, m_texture);
    
    if (m_normalTexture.m_pTexture) {
      inGAPI->setShaderResource(1, m_normalTexture);
    }

    if (m_roughnessTexture.m_pTexture) {
      inGAPI->setShaderResource(2, m_roughnessTexture);
    }

    if (m_metalicTexture.m_pTexture) {
      inGAPI->setShaderResource(3, m_metalicTexture);
    }

    m_model.draw(inGAPI);
  }



  Model m_model;
  Texture m_texture;
  Texture m_normalTexture;
  Texture m_roughnessTexture;
  Texture m_metalicTexture;
};