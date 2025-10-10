/**
 * @file    ofLogger.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    02/08/2023
 * @brief   A log creator for the engine.
 */
 
/**
 * @include
 */
#pragma once
#include "PrerequisiteGraficas.h"
#include "Module.h"
#include <fstream>
#include <ctime>


namespace ERROR_TYPE {
  enum E {
    kModelLoading = 0,
    kWindowCreation,
    kPluginLoading,
    kAnimationLoading,
    kTextureCreation,
    kBufferCreation,
    kBlendStateCreation,
    kDepthStencilStateCreation,
    kSamplerCreation,
    kUAVCreation,
    kDecodingFile,
    kEncodingFile,
    kResizeTextures,
  };
}


class Logger : public Module<Logger>
{
 public:
  //Constructor
  Logger();

  //Destructor
  ~Logger() = default;

  /**
   * @brief      Set an error to the list.
   * @param      inProcess     The process where the error occurred.
   * @param      inError       The type of error.
   */
  void
  SetError(ERROR_TYPE::E inProcess, String inError);

  /**
   * @brief      Set an Message to the logger and console.
   * @param      inMessage       The type of error.
   */
  void
  SetMessage(String inMessage);
  
  /**
   * @brief      Set an Message for debug (console only).
   * @param      inMessage       The type of error.
   */
  void
  SetDebugLine(String inMessage);

  /**
   * @brief      Send the error to a file.
   */
  void
  SendToFile();

  /**
   * @brief      Send the errors to display in the console.
   */
  void
  sendToConsole();

 private:
  /**
   * @brief      The id of the log.
   */
  String m_logId;
  
  /**
   * @brief      The path to save the file.
   */
  String m_path = ("Log/LogDefault.txt");

  /**
   * @brief      The number of errors.
   */
  uint32 m_numErrors = 0;

  /**
   * @brief      The number of messages.
   */
  uint32 m_numMessages = 0;

  /**
   * @brief      Errors by type.
   */
  Map<ERROR_TYPE::E, String> m_errors;

  /**
   * @brief      Messages by id (Date+Time).
   */
  Map<int32, String> m_messages;
};

Logger&
g_logger();