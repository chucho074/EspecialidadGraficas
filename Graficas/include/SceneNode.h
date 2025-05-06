/**
 * @file    SceneNode.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    29/4/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Object.h"

class SceneNode : public Object {
 public:
  SceneNode() = default;
  ~SceneNode() = default;
 
  virtual void
  addChild(const SPtr<SceneNode>& inChild);

  virtual void
  removeChild(const SPtr<SceneNode>& inChild);

  template<typename T, typename... Args>
  static SPtr<T> 
  createSceneObject(Args&&... args);

  
  Vector<SPtr<SceneNode>> m_children;

 protected:
  WPtr<SceneNode> m_parent;

  SPtr<SceneNode> 
  sharedFromSelf() {
    return m_selfPtr.lock();
  }

  void
  setSelfPtr(const SPtr<SceneNode>& inSelf) {
    m_selfPtr = inSelf;
  }

  WPtr<SceneNode> m_selfPtr;
};

//////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename... Args>
inline SPtr<T> 
SceneNode::createSceneObject(Args&&... args) {
  //Checks if T is derived from SceneNode
  static_assert(std::is_base_of<SceneNode, T>::value, "T must be derived from SceneNode"); 

  //auto node = SPtr<T>(new T(std::forward<Args>(args)...));
  auto node = make_shared<T>(std::forward<Args>(args)...); //Always perfer make_shared
  node->setSelfPtr(node);

  return node;
}