/**
 * @file    Actor.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#pragma once
#include "SceneNode.h"
#include "Transform.h"
#include "GraphicsAPI.h"

namespace ActorType {
  enum E {
    kActor = 0,
    kProp,
    kLight,
    kCamera,

  };
}

class Actor : public SceneNode {
 public:
  Actor() {
    setName("Actor");
  }
  virtual ~Actor() = default;

  virtual void
  init(Vector3 inPos, 
       Vector3 inScale = Vector3::UNIT,
       Vector3 inRotation = Vector3::ZERO) {
    setPosition(inPos);
    setScale(inScale);
    setRotation(inRotation);
  }

  virtual void 
  update(float inDT) override {
    MY_UNREFERENCED_PARAMETER(inDT);
    updateTransforms();
  }

  virtual void
  draw(bool inWithMaterial) {
    MY_UNREFERENCED_PARAMETER(inWithMaterial);
  }

  void
  setPosition(Vector3 inPosition) {
    m_transform.setLocalPosition(inPosition);
  }
  
  void
  setScale(Vector3 inScale) {
    m_transform.setLocalScale(inScale);
  }

  void
  setRotation(Vector3 inRotation) {
    m_transform.setLocalRotation(inRotation);
  }

  
  void
  updateTransforms() {
    if(!m_parent.expired()) {
      SPtr<Actor> parentActor = static_pointer_cast<Actor>(m_parent.lock());

      Vector3 parentRotation = parentActor->m_transform.getGlobalRotation();
      Vector3 rotatedPosition;

      rotatedPosition.x = m_transform.getLocalPosition().x * cos(parentRotation.y) - m_transform.getLocalPosition().z * sin(parentRotation.y);
      rotatedPosition.y = m_transform.getLocalPosition().y + parentActor->m_transform.getGlobalPosition().y;
      rotatedPosition.z = m_transform.getLocalPosition().x * sin(parentRotation.y) + m_transform.getLocalPosition().z * cos(parentRotation.y);
      
      m_transform.setGlobalPosition(parentActor->m_transform.getGlobalPosition() + rotatedPosition);
      m_transform.setGlobalRotation(parentActor->m_transform.getGlobalRotation() + m_transform.getLocalRotation());

      Vector3 tmpScale;
      tmpScale.x = m_transform.getLocalScale().x * parentActor->m_transform.getGlobalScale().x;
      tmpScale.y = m_transform.getLocalScale().y * parentActor->m_transform.getGlobalScale().y;
      tmpScale.z = m_transform.getLocalScale().z * parentActor->m_transform.getGlobalScale().z;
      m_transform.setGlobalScale(tmpScale);
    }
    else {
      m_transform.setGlobalPosition(m_transform.getLocalPosition());
      m_transform.setGlobalRotation(m_transform.getLocalRotation());
      m_transform.setGlobalScale(m_transform.getLocalScale());
    }
  }

  //
  String
  getTypeAsString() const {
    switch(m_type) {
      case ActorType::kActor: return "Actor"; break;
      case ActorType::kProp: return "Prop"; break;
      case ActorType::kLight: return "Light"; break;
      case ActorType::kCamera: return "Camera"; break;
      default: return "Unknown"; break;
    }
  }

  Transform m_transform;

  ActorType::E m_type = ActorType::kActor;

  bool isActive = true;
};
