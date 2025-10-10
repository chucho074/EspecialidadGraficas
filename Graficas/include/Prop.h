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
#include "Camera.h"
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

class LightProp : public Prop {
 public:
  LightProp() { 
    m_shadowCamera = make_shared<Camera>();
    m_type = ActorType::kLight;
    m_name = "Light";
    m_model.createSphere(50);
  }

  ~LightProp() = default;


  void
  init(Vector3 inPos, 
       Vector3 inScale = Vector3::UNIT, 
       Vector3 inRotation = Vector3::ZERO) override {
    Actor::init(inPos, inScale, inRotation);  
    m_shadowCamera->setLookAt(inPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
    //g_shadowCamera->setOrthographic(-0.75f, 0.75f, -0.75f, 0.75f, 0.1f, 500.f);  //El bueno 
    m_shadowCamera->setOrthographic(-5.f, 5.f, -5.f, 5.f, 0.01f, 10000.f);  //Testing
  }

  void
  update(float inDT) override {
    Actor::update(inDT);
    m_shadowCamera->m_position = m_transform.getGlobalPosition();
  }

  void
  draw(bool inWithMaterial) override {
    auto& shaderManager = g_shaderManager();

    shaderManager.setTransform(m_transform);
    
    m_model.draw(inWithMaterial);
  }

 public:
  
  SPtr<Camera> m_shadowCamera;

  bool isActiveLight = true;
  Vector3 lightColor = Vector3(1.f, 1.f, 1.f);
  
  float lightIntensity = 1.f;
  
  float lightRadius = 10.f;

};

class LightProp : public Prop {
 public:
  LightProp() { 
    m_shadowCamera = make_shared<Camera>();
    m_type = ActorType::kLight;
    m_name = "Light";
    m_model.createSphere(50);
  }

  ~LightProp() = default;

};

class LightProp : public Prop {
 public:
  LightProp() { 
    m_shadowCamera = make_shared<Camera>();
    m_type = ActorType::kLight;
    m_name = "Light";
    m_model.createSphere(50);
  }

  ~LightProp() = default;


  void
  init(Vector3 inPos, 
       Vector3 inScale = Vector3::UNIT, 
       Vector3 inRotation = Vector3::ZERO) override {
    Actor::init(inPos, inScale, inRotation);  
    m_shadowCamera->setLookAt(inPos, Vector3(0, 0, 0), Vector3(0, 1, 0));
    //g_shadowCamera->setOrthographic(-0.75f, 0.75f, -0.75f, 0.75f, 0.1f, 500.f);  //El bueno 
    m_shadowCamera->setOrthographic(-5.f, 5.f, -5.f, 5.f, 0.01f, 10000.f);  //Testing
  }

  void
  update(float inDT) override {
    Actor::update(inDT);
    m_shadowCamera->m_position = m_transform.getGlobalPosition();
  }

  void
  draw(bool inWithMaterial) override {
    auto& shaderManager = g_shaderManager();

    shaderManager.setTransform(m_transform);
    
    m_model.draw(inWithMaterial);
  }

 public:
  
  SPtr<Camera> m_shadowCamera;

  bool isActiveLight = true;
  Vector3 lightColor = Vector3(1.f, 1.f, 1.f);
  
  float lightIntensity = 1.f;
  
  float lightRadius = 10.f;

  //Light type
};