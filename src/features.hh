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

#ifndef FEATURES_H
#define FEATURES_H

#include <list>
#ifdef OUTPUT_XML
#include <libxml/tree.h>
#endif
#include "flags.hh"
#include "serializable.hh"
#include "id.hh"
#include "ipointer.hh"
#include "vartable.hh"

class Features:
  public Id,
  public Flags,
  public Serializable,
  public enable_shared_from_this< class Features > {
  
public:
  typedef std::list< featurePtr > listFeatures;

public:
  static featuresPtr _nil;
  static featuresPtr _bottom;

private:
  listFeatures features;
  unsigned int pred;
  std::string form;
  
  Features(featurePtr);

  static featuresPtr createBottom(void);
  static featuresPtr createNil(void);
  void setId(idType);
  const std::string makeSerializationId(void);

public:
  ~Features();
  static featuresPtr create(featurePtr=featurePtr());

  void putId(unsigned int id);

  void addFeature(featurePtr, bool=false);
  void addFeatures(featuresPtr, bool=false);
  listFeatures::iterator erase(listFeatures::iterator i);

  const size_t size(void) const;
  listFeatures::iterator begin(void);
  listFeatures::iterator end(void);
  featurePtr front(void) const;

  void print(std::ostream &, bool=true, bool=false) const;
#ifdef OUTPUT_XML
  void toXML(xmlNodePtr);
#endif
  featuresPtr clone(void);
  valuePtr find(bitsetPtr) const;

  const bool buildEnvironment(environmentPtr, featuresPtr, bool, bool);
  void subFlags(const std::bitset< NBRFLAGS > &);
  const unsigned int assignPred(void);
  std::string assignForm(void);
  const bool renameVariables(unsigned int);
  const bool isNil(void) const;
  const bool isBottom(void) const;
  void enable(statementPtr, itemPtr, bool &, bool);
  const bool subsumes(featuresPtr, environmentPtr);
  void deleteAnonymousVariables(void);
  const bool findVariable(bitsetPtr);
};

#endif // FEATURES_H

