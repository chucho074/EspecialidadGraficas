/**
 * @file    giTime.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    19/06/2025
 * @brief   A basic use of the time.
 */

/**
 * @include
 */
#pragma once
#include "stdHeaders.h"
#include "Module.h"
#include "Timer.h"

/**
 * @class    Time.
 * @brief    .   
 */
class giTime : public Module<giTime>
{
 public:
  //Default Constructor.
  giTime();
  
  //Destructor.
  ~giTime() = default;
  
  /**
   * @brief 
   */
  void
  update();

  void
  startTimer();

  void
  stopTimer();

  void
  restartTimer();

  /**
   * @brief    Gets the time elapsed since application start.
   * @return   Returns .
   */
  float
  getTime() const;

  static const double MICRO_TO_SEC;
 
 private:

  SPtr<Timer> m_timer;

  float m_deltaTime;

  uint64 m_lastFrameTime = 0u;

  uint64 m_timeSinceStartMs = 0u;

  bool m_firstFrame = true;

  bool m_timerRunning = false;

  /**
   * @brief    Time since start in seconds.
   */
  float m_timeSinceStart = 0.0f;


};

giTime&
g_time();

