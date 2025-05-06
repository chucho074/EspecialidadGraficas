/**
 * @file    UID.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#include "UUID.h"
#include <random>


static std::random_device s_randomDevice;
static std::mt19937_64 s_Engine(s_randomDevice());
static std::uniform_int_distribution<uint64> s_UniformDistribution;

UID::UID()
  : m_uuid(s_UniformDistribution(s_Engine)) {

}

UID::UID(uint64 inData) {
  m_uuid = inData;
}

const UID
UID::ZERO = UID(0);