/**
 * @file    SceneGraph.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    29/4/2025
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "SceneNode.h"
#include "Actor.h"
#include "UUID.h"

class SceneGraph {
 public:
  SceneGraph() = default;
  ~SceneGraph() = default;

  /**
   * @brief      .
   * @param      inParent      .
   * @param      args          The rest of the arguments for the templates.
   * @return     .
   */
  template <typename T, typename... Args>
  SPtr<Actor>
  spawnActor(const SPtr<SceneNode>& inParent, Args&&... args);

  /**
   * @brief    Adds an actor to the list.
   * @param    inActor       The actor reference to add.
   * @param    inParent      The parent to atatch the actor.
   */
  void
  addActor(const SPtr<Actor>& inActor,
           SPtr<SceneNode> inParent);
  /**
   * @brief    Obtain the actor childs of the root.
   * @return   Returns the list of the actors who there parent is the root.
   */
  Vector<SPtr<Actor>>
  getActorsFromRoot();
 
  /**
   * @brief    Gets a list of actors by the parent.
   * @param    inParent      The reference of the parent to search.
   */
  List<SPtr<SceneNode>>
  getNodesByParent(WPtr<SceneNode> inParent);

  /**
   * @brief    Gets the root of the scene.
   * @return   Returns the root of the scene.
   */
  SPtr<SceneNode>
  getRoot();

  /**
   * @brief    Delete that actor, asign the childs to the parent.
   */
  void
  deleteActor(const SPtr<Actor>& inActor);


  /**
   * @brief    Clears the list of actors of the Graph.
   */
  void
  clearGraph();

 private:

  /**
   * @brief    The root of the Scene.
   */
  SPtr<SceneNode> m_root;

 public:
   
  /**
   * @brief    The name of the active Scene Graph.
   */
  String m_sceneName = "Untitled scene";

  /**
   * @brief    The ID of the scene.
   */
  UUID m_sceneID;

  /**
   * @brief    The number of the actors.
   */
  uint32 m_numActors;


};
