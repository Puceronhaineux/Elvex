/* **************************************************
 *
 * ELVEX
 *
 * Copyright 2018 LABRI, 
 * CNRS (UMR 5800), the University of Bordeaux,
 * and the Bordeaux INP
 *
 * Author: Lionel Clément,
 * Labri -- 351, cours de la Libération
 * 33405 Talence Cedex - France
 * lionel.clement@labri.fr
 * 
 * This file is part of ELVEX.
 *
 ************************************************** */

#ifndef FORESTIDENTIFIER_H
#define FORESTIDENTIFIER_H

#include <ostream>
#include "serializable.hh"
#include "ipointer.hh"

class ForestIdentifier:
  //public Serializable,
  public enable_shared_from_this< ForestIdentifier > {  
  
private:
  
  unsigned int code;
  unsigned int from;
  unsigned int to;
  std::string featuresSerialID;
  
  ForestIdentifier(unsigned int, std::string, unsigned int, unsigned int);  
  const std::string makeSerializationId(void);

public:
  ~ForestIdentifier();
  static forestIdentifierPtr create(unsigned int, std::string, unsigned int, unsigned int);  
  
  const unsigned int getFrom(void){return from;};
  const unsigned int getTo(void){return to;};
  const unsigned int getCode(void){return code;};
  void print(std::ostream &) const;
  
  const bool operator <(const ForestIdentifier &) const;
  const bool operator !=(const ForestIdentifier &) const;

  struct Less {
    const bool operator()(const forestIdentifierPtr, const forestIdentifierPtr) const;
  };
  
};

#endif // FORESTIDENTIFIER_H
