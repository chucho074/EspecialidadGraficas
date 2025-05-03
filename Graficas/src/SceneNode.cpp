/**
 * @file    SceneNode.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#include "SceneNode.h"


void 
SceneNode::addChild(const SPtr<SceneNode>& inChild) {
  if(!inChild) {
    std::cerr << "Error: Attempting to add a null child to a SceneObject." << std::endl;
    return;
  }
  m_children.push_back(inChild);
  inChild->m_parent = sharedFromSelf();

}

void 
SceneNode::removeChild(const SPtr<SceneNode>& inChild) {
  if(!inChild) {
    std::cerr << "Error: Attempting to remove a null child from a SceneObject." << std::endl;
    return;
  }

  auto it = std::remove(m_children.begin(), m_children.end(), inChild);
  if(it != m_children.end()) {
    m_children.erase(it, m_children.end());
    inChild->m_parent.reset();
  }
  else {
    std::cerr << "Error: Attempting to remove a child that is not a child of this SceneObject." << std::endl;
  }  
}
