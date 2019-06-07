/* **************************************************
 *
 * ELVEX
 *
 * Copyright 2019 LABRI, 
 * CNRS (UMR 5800), the University of Bordeaux,
 * and the Bordeaux INP
 *
 * Author: 
 * Lionel Clément
 * LaBRI -- Université Bordeaux 
 * 351, cours de la Libération
 * 33405 Talence Cedex - France
 * lionel.clement@labri.fr
 * 
 * This file is part of ELVEX.
 *
 ************************************************** */

#ifndef ITEM_H
#define ITEM_H

#include <set>
#include <list>
#include <fstream>
#include <climits>
#include "features.hh"
#include "flags.hh"
#include "id.hh"
#include "ipointer.hh"
#include "item.hh"

class Item:
  public Id,
  public Flags,
  public enable_shared_from_this< class Item > {
  
private:
  class Rule *rule;
  unsigned int index; // the \bullet position
  std::vector<unsigned int> indexTerms; // which term in a disjunction
  statementsPtr statements;
  
  std::set< unsigned int > refs; // item from which this one is derived
  std::vector< bool > seen;
  std::vector< unsigned int > ranges;
  featuresPtr inheritedFeatures; //↑
  listFeaturesPtr inheritedSonFeatures; //↓
  featuresPtr synthesizedFeatures; //⇑
  listFeaturesPtr synthesizedSonFeatures; //⇓
  std::vector< forestIdentifierPtr > forestIdentifiers;
  environmentPtr environment;
  
  bool
    s_id = false,
    s_ruleId = false,
    s_ruleSerialId = false,
    s_flags = false,
    s_refs = false,
    s_seen = false,
    s_item = true,
    s_index = false,
    s_indexTerms = false,
    s_terms = false,
    s_ranges = false,
    s_forestIdentifiers = false,
    s_inheritedFeatures = true,
    s_inheritedSonFeatures = true,
    s_synthesizedFeatures = true,
    s_synthesizedSonFeatures = true,
    s_statements = true,
    s_environment = true;

  Item (class Rule *,
	unsigned int,
	unsigned int,
	statementsPtr);
  
  Item (class Rule *,
	unsigned int,
	std::vector<unsigned int> &,
	statementsPtr);
  void setId(idType);
  
public:
  virtual ~Item();
  static itemPtr create(class Rule *,
			unsigned int=UINT_MAX,
			unsigned int=0,
			statementsPtr=statementsPtr());
  
  static itemPtr create (class Rule *,
			 unsigned int,
			 std::vector<unsigned int> &,
			 statementsPtr);
  
  const unsigned int getLineno() const;
  const std::string &getFilename() const;
  class Term *getCurrentTerm(void) const;
  class Terms *getCurrentTerms() const;
  class Terms *getTerms(unsigned int) const;
  void setCurrentTerms(class Terms *);
  
  class Rule *getRule(void) const;
  void setRule(class Rule *);
  unsigned int getIndex(void) const;
  void setIndex(unsigned int);
  std::vector<unsigned int> &getIndexTerms(void);
  
  class Term *getLhs(void) const;
  std::vector<class Terms*> &getRhs(void) const;
  
  statementsPtr getStatements(void);
  
  std::set<unsigned int> &getRefs(void);
  void setRefs(std::set<unsigned int>&);
  void addRef(unsigned int);
  void addRefs(std::set<unsigned int>&);
  
  std::vector< bool > &getSeen(void);
  void setSeen(std::vector< bool > &);
  bool isSeen(unsigned int) const;
  void setSeen(unsigned int, bool);

  void addItem(std::map<int, itemPtr > &, int, itemPtr );
  
  std::vector<unsigned int> & getRanges(void);
  void addRanges(unsigned int);
  void addRanges(std::vector<unsigned int> &);
  
  std::vector< forestIdentifierPtr > & getForestIdentifiers(void);
  void addForestIdentifiers(unsigned int i, forestIdentifierPtr );
  void addForestIdentifiers(std::vector<forestIdentifierPtr > &);
  
  listFeaturesPtr getSynthesizedSonFeatures(void);
  void setSynthesizedSonFeatures(listFeaturesPtr );
  
  listFeaturesPtr getInheritedSonFeatures(void);
  void setInheritedSonFeatures(listFeaturesPtr );
  
  featuresPtr getSynthesizedFeatures(void) const;
  void setSynthesizedFeatures(featuresPtr);
  
  featuresPtr getInheritedFeatures(void) const;
  void setInheritedFeatures(featuresPtr);
  
  void buildSynthesizedFeatures(class Synthesizer *);
  void buildInheritedSonFeatures(class Synthesizer *);
  
  environmentPtr getEnvironment(void) const;
  void setEnvironment(environmentPtr);
  void addEnvironment(environmentPtr);
  void addEnvironment(environmentPtr, environmentPtr);
  
  void addStatements(statementsPtr);
  
  void print(std::ostream &) const;
  itemPtr clone(const std::bitset<NBRFLAGS> &savedFlags);
  bool isCompleted(void);
  bool isStarted(void);
  void successor(itemSetPtr, class Synthesizer *, bool &);
  void defaultInheritedSonFeatures (void);
  void apply(itemSetPtr, class Synthesizer *);
  
  // compare deux items
  struct Less {
    const bool operator() (itemPtr, itemPtr) const;
  };
  
};  
#endif // ITEM_H
