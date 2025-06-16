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
  m_editorCamera.setLookAt(Vector3(-5, 5, 10), Vector3(0, 0, 0), Vector3(0, 1, 0));
  m_editorCamera.setPerspectiveHalf(3.1415926353f / 4.f, Vector2(1280, 720), 0.1f, 1000.f);
}

void 
SceneGraph::update(float inDT) {
  m_editorCamera.move(inDT);

  for(auto& actor : m_actors) {
    actor->update(inDT);
  }
}

void 
SceneGraph::draw(bool inWithMaterial) {
  for(auto& actor : m_actors) {
    actor->draw(inWithMaterial);
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
