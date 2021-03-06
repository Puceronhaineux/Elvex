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

#ifndef VARTABLE_H
#define VARTABLE_H

#include <bitset>
#include <map>
#ifdef OUTPUT_XML
#include <libxml/tree.h>
#endif
#include "bitset.hh"
#include "ipointer.hh"

class Vartable {

public:

  static const unsigned int _END_ = 0;
  static const unsigned int _STARTTERM_ = 1;
  static const unsigned int _EMPTY_ = 2;
  static const unsigned int _OPEN_ = 3;
  static const unsigned int _FIRSTID_ = 4;
  
public:
  static unsigned int intToStrIndex;
  static std::bitset<MAXBITS> varTableIndex;
  static std::map<std::string, bitsetPtr, std::less<std::string> > varTable;
  static std::map<unsigned int, std::string, std::less<unsigned int> > intToStrTable;
  static std::map<std::string, unsigned int, std::less<std::string> > strToIntTable;
  
  static void init();
  static bitsetPtr varTableAdd(std::string); // string -> bit
  static unsigned int strToInt(std::string); // string -> integer
  static std::string intToStr(unsigned int);
};

#endif // VARTABLE_H
