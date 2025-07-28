/**
 * @file    giTimer.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    27/06/2021
 * @brief   A basic description of the what do the doc.
 */
 
/**
 * @include
 */
#pragma once
#include "stdHeaders.h"
#include <chrono>

using std::chrono::time_point;
using std::chrono::high_resolution_clock;
using namespace std::chrono;


/**
 * @enum     TimeType
 * @brief    The ways to count the time.
 */
namespace TimeType {
  enum E {
    kNanoSeconds,
    kMicroSeconds,
    kMilliSeconds,
    kSeconds,
    kMinutes,
    kHours
  };
}

/**
 * @class    Timer.
 * @brief    .   
 */
class Timer
{
 public:
  //Default Constructor.
  Timer() {
    restart();
  }

  //Destructor.
  ~Timer() = default;

  /**
   * @brief    Restart the time counter.
   */
  void 
  restart() {
    m_startTime = m_clock.now();
  }

  /**
   * @brief    Get the elapsed time in nanoseconds.
   * @return   Returns the time in Nanoseconds.
   */
  uint32
  getNanoseconds() {
    return getTimeByType(TimeType::kNanoSeconds);
  }

  /**
   * @brief    Get the elapsed time in microseconds.
   * @return   Returns the time in Microseconds.
   */
  uint32 
  getMicroseconds() {
    return getTimeByType(TimeType::kMicroSeconds);
  }

  /**
   * @brief    Get the elapsed time in Milliseconds.
   * @return   Retunrs the time in Miliseconds
   */
  uint32
  getMilliseconds() {
    return getTimeByType(TimeType::kMilliSeconds);
  }

 protected:
  uint32
  getTimeByType(TimeType::E inTimeType) {
  
    auto newTime = m_clock.now();
    duration<double> dur = newTime - m_startTime;

    switch (inTimeType) {
      //Nano seconds case
      case TimeType::kNanoSeconds:{
        //nanoseconds startTimeNs = m_startTime.time_since_epoch();
        //return duration_cast<milliseconds>(startTimeNs).count();
        return 0;
      }
      case TimeType::kMicroSeconds: {
        return (uint32)duration_cast<microseconds>(dur).count();
      }

      case TimeType::kMilliSeconds: {
        return (uint32)duration_cast<milliseconds>(dur).count();
      }
      case TimeType::kSeconds: {
        return (uint32)duration_cast<seconds>(dur).count();
      }
      case TimeType::kMinutes: {
        return (uint32)duration_cast<seconds>(dur).count() / 60;
      }
      case TimeType::kHours: {
        return (uint32)duration_cast<seconds>(dur).count() / 3600;
      }

    }
    return 0;
  }
  
  

  high_resolution_clock m_clock;
  time_point<high_resolution_clock> m_startTime;

 private:

};