/**
 * @file    Logger.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    02/08/2023
 * @brief   A log creator for the engine.
 */
 
/**
 * @include
 */
#include "Logger.h"

Logger::Logger() {
  time_t timer;
  std::tm timeinfo;
  time(&timer);
  localtime_s(&timeinfo, &timer);

  m_logId = timeinfo.tm_hour + "-" + timeinfo.tm_min, "-" + timeinfo.tm_mday, "-" +
            timeinfo.tm_mon, "-" + timeinfo.tm_yday;
  //m_path =  ("Log/Log" + m_logId + ".txt");
}

void
Logger::SetError(ERROR_TYPE::E inProcess, String inError) {
  ++m_numErrors;
  m_errors.insert(m_errors.end(), 
                  Pair<ERROR_TYPE::E, 
                  String>(inProcess, inError));
  
}

void
Logger::SetMessage(String inMessage) {
  time_t timer;
  std::tm timeinfo;
  time(&timer);
  localtime_s(&timeinfo, &timer);

  int32 tmpTime = timeinfo.tm_year + timeinfo.tm_mon + timeinfo.tm_mday + timeinfo.tm_hour 
                  + timeinfo.tm_min + timeinfo.tm_sec;
  ++m_numMessages;
  m_messages.insert(m_messages.end(),
                    Pair<int32, String>(tmpTime, inMessage));
}

void
Logger::SetDebugLine(String inMessage) {
  ConsoleOut << inMessage << ConsoleLine;
}

void
Logger::sendToConsole() {
  ConsoleOut << "> The number of errors in total is: " + m_numErrors << ConsoleLine;
  for (auto errors : m_errors) {
    ConsoleOut << errors.second << ConsoleLine;
  }
  ConsoleOut << "> The number of messages in total is: " + m_numMessages << ConsoleLine;
  for (auto messages : m_messages) {
    ConsoleOut << messages.second << ConsoleLine;
  }
}

void 
Logger::SendToFile() {
  std::fstream fs;
  fs.open(m_path, std::fstream::out );//| std::fstream::app);
  fs << "The number of errors in total is: " + m_numErrors;
  for(auto errors : m_errors) {
    fs << errors.second;
  }
  sendToConsole();
}

Logger&
g_logger() {
  return Logger::instance();
}