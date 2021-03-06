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

#ifndef STATEMENTS_H
#define STATEMENTS_H

#include <list>
#include <bitset>
#include <vector>

#include "flags.hh"
#include "id.hh"
#include "serializable.hh"
#include "ipointer.hh"

class Statements:
  public Id,
  public Flags,
  public Serializable,
  public enable_shared_from_this< class Statements > {

private:
  statementPtr guard;
  std::list< statementPtr > statements;
  Statements(statementPtr);
  const std::string makeSerializationId(void);
  void setId(idType);

public:
  ~Statements();
  static statementsPtr create(statementPtr=statementPtr());
  
  void incPtr(void);
  std::list<statementPtr > &getStatements(void);
  
  size_t size(void);
  std::list<statementPtr >::const_iterator begin(void) const;
  std::list<statementPtr >::const_iterator end(void) const;
  
  void addStatement(statementPtr);
  void renameVariables(unsigned int);
  void print(std::ostream &, int=0) const;
  statementsPtr clone(const std::bitset<NBRFLAGS>&savedFlags) const;
  void apply(itemPtr, bool &, bool);
  void lookingForAssignedInheritedSonFeatures(std::vector< bool > &);
  void enable(itemPtr, bool &, bool);
  bool findVariableElsewhere(statementPtr, bitsetPtr);
};

#endif // STATEMENTS_H
