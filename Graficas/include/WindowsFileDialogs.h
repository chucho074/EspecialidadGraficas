/**
 * @file    WindowsImplementations.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    27/09/2022
 * @brief   A basic description of the what do the doc.
 */

 /**
  * @include
  */
#pragma once
#include "PrerequisiteGraficas.h"

class WindowsFileDialogs {
 public:

  /**
   * @brief    For opens the open file dialog.
   * @param    inWindowHandle The handle of the app window.
   * @return   Returns the path of the file selected in the dialog.
   */
  static String
  openFileDialog(void* inWindowHandle, const char* inFilters = m_fileFiltersProject);

  /**
   * @brief    For save as a new file in a dialog.
   * @param    inWindowHandle The handle of the app window.
   * @return   Returns the path where the file is going to be saved.
   */
  static String
  saveFileDialog(void* inWindowHandle, const char* inFilters = " ");

  /**
   * @brief    For selecting a folder to use in a dialog.
   * @return   The selected path.
   */
  static Path
  selectFolderDialog();

  static inline const char* m_fileFilters = "All\0*.*\0Text\0*.txt\0giEngine Scenes\0*.giScene\0";
  static inline const char* m_fileFiltersProject = "giEngine Projects\0*.giProject\0";
  static inline const char* m_fileFiltersImage = "Images\0*.png.jpeg*\0PNG\0*.png\0JPEG\0*.jpeg\0";
  static inline const char* m_fileFiltersModels = "Models\0*.*\0OBJ\0*.obj\FBX\0*.fbx\0";
};
