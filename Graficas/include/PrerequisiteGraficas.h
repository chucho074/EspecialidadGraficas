/**
 * @file    PrerequisiteGraficas.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    22/02/25
 * @brief   .
 */

/**
 * @include
 */
#pragma once
#include "stdHeaders.h"
#include "mathObjects.h"

#define SAFE_RELEASE(x) if(x != nullptr) x->Release(); x=nullptr;
#define SAFE_DELETE(x) if(x != nullptr) delete(x); x=nullptr;
