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
  draw() override {
    auto& GAPI = g_graphicsAPI();

    m_material.draw();

    if(!m_model.m_meshes.empty()) {
      m_model.draw();
    }
  }



  Model m_model;

  PBRMaterial m_material;
};