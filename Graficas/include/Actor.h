/**
 * @file    Actor.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       edgv24c.jmoral@uartesdigitales.edu.mx
 * @date    30/4/2025
 */

/**
 * @include
 */
#pragma once
#include "SceneNode.h"

class Actor : public SceneNode {
 public:
	Actor();
	~Actor();

  virtual void 
  update(float inDT) override;



 protected:
	
 private:
};
