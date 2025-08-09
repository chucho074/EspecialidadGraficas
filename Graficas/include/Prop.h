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
#include "Material.h"
#include "GraphicsAPI.h"
#include "ShaderManager.h"


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
  draw(bool inWithMaterial) override {
    auto& shaderManager = g_shaderManager();

    shaderManager.setTransform(m_transform);

    if(inWithMaterial) {
      //m_material.draw();
    }

    m_model.draw(inWithMaterial);
  }



  Model m_model;

  //sPBRMaterial m_material;
};