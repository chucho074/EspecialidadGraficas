/**
 * @file    UI.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    08/10/2025
 * @brief   In charge of the UI elements.
 */

/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Module.h"




class Actor;

class UI : public Module<UI>{
 public:
  UI();
  ~UI();

  void
  init(void* inHandle);

  /**
   * @brief      Make the ImGui process to render an calls to render the user UI.
   */
  void
  render();

  void
  destroy();


 protected:
  /**
   * @brief      Calls the UI elements to render.
   */
  void
  renderUI();

  void* m_handle = nullptr;

  //
  void
  renderNode(SPtr<Actor> inNode);

	int m_rootFlags;
  
  int m_treeSelectableFlags;
  
  int m_leafFlags;

  int32 m_meshIndex;
};

UI &
g_ui();