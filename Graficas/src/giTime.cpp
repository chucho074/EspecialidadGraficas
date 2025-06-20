/**
 * @file    giTime.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    19/06/2025
 */

/**
 * @include
 */
#include "giTime.h"

const double giTime::MICRO_TO_SEC = 1.0 / 1000000.0;

giTime::giTime() {
  m_timer = make_unique<Timer>();
}  

void 
giTime::startTimer() {
  m_timerRunning = true;
  m_lastFrameTime = m_timer->getMicroseconds();

}

void 
giTime::stopTimer() {
  update();
  m_timerRunning = false;
}

void 
giTime::restartTimer() {
  m_timer->restart();
  m_timerRunning = true;
  m_timeSinceStartMs = 0;
  m_timeSinceStart = 0.0f;
  m_deltaTime = 0.0f;
  m_firstFrame = true;
  m_lastFrameTime = m_timer->getMicroseconds();

}

void 
giTime::update() {

  uint64 currentFrameTime = m_timer->getMicroseconds();

  if (!m_firstFrame) {
    m_deltaTime = static_cast<float>((currentFrameTime - m_lastFrameTime) *
                                      MICRO_TO_SEC);
  }
  else {
    m_deltaTime = 0.0f;
    m_firstFrame = false;
  }

  m_timeSinceStartMs = static_cast<uint64>(currentFrameTime / 1000);
  m_timeSinceStart = m_timeSinceStartMs / 1000.0f;
  m_lastFrameTime = currentFrameTime;
}

float
giTime::getTime() const {
  return m_deltaTime;
}

giTime&
g_time() {
  return giTime::instance();
}
