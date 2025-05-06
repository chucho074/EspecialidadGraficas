/**
 * @file    SceneGraph.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#include "SceneGraph.h"

void 
SceneGraph::init() {
  m_root = SceneNode::createSceneObject<Actor>();
  m_root->setName("Root");
  m_root->init({0.f, 0.f}, {1.f, 1.f}, 0.f);
}

void 
SceneGraph::update(float inDT) {
  for(auto& actor : m_actors) {
    actor->update(inDT);
  }
}

void 
SceneGraph::draw(const UPtr<GraphicsAPI>& inGAPI) {
  for(auto& actor : m_actors) {
    actor->draw(inGAPI);
  }
}

void
SceneGraph::removeActor(SPtr<Actor> inActor) {
  auto it = std::remove(m_actors.begin(), m_actors.end(), inActor);
  if(it != m_actors.end()) {
    m_actors.erase(it, m_actors.end());
  }
}

Vector<SPtr<Actor>>& 
SceneGraph::getActorsFromRoot() {
  Vector<SPtr<Actor>> tmpVector;
  for(auto& nodes : m_root->m_children) {
    tmpVector.push_back(static_pointer_cast<Actor>(nodes));
  }

  return tmpVector;
}

List<SPtr<SceneNode>>& 
SceneGraph::getNodesByParent(WPtr<SceneNode> inParent) {
  if(SPtr<SceneNode>(nullptr) == inParent.lock()) {
    List<SPtr<SceneNode>> tmpList;
    tmpList.push_back(m_root);
    return tmpList;
  }

  return getNodesByParent(inParent);
}

SPtr<SceneNode> 
SceneGraph::getRoot() {
  return m_root;
}

void 
SceneGraph::deleteActor(const SPtr<Actor>& inActor) {
  m_root->removeChild(inActor);
}

void 
SceneGraph::clearGraph() {
  
}
