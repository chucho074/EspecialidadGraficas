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
  Prop() { 
    m_type = ActorType::kProp;
  }

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
    
    m_model.draw(inWithMaterial);
  }

  void 
  setModel(Model& inModel) {
    m_model = inModel;
    m_name = m_model.m_path.stem().string();
  }

  bool
  createFromFile(const Path& inPath) {
    if(!m_model.loadFromFile(inPath)) {
      return false; 
    }
    m_model.createBuffers();
    m_name = m_model.m_path.stem().string();
    return true;
  }
 
  Model m_model;

};