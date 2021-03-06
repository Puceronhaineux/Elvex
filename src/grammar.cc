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

#include "grammar.hh"
#include "term.hh"
#include "terms.hh"
#include "vartable.hh"
#include "messages.hh"
#include "synthesizer.hh"
#include "entries.hh"

/* **************************************************
 *
 ************************************************** */
Grammar::Grammar(void) {
  NEW;
  this->firstRule = NULL;
  this->startTerm = NULL;
  this->idMax = 0;
}

/* **************************************************
 *
 ************************************************** */
Grammar::~Grammar(void) {
  terminals.clear();
  nonTerminals.clear();
  for (ruleSet::iterator iterRules = rulesBegin();
       iterRules != rulesEnd();
       ++iterRules) {
    class Rule *tmp = *iterRules;
    if (tmp)
      delete (tmp);
  }
  rules.clear();
  if (startTerm){
    delete startTerm;
    startTerm = NULL;
  }
  if (firstRule){
    delete firstRule;
    firstRule = NULL;
  }
  DELETE;
}

/* **************************************************
 *
 ************************************************** */
std::set<unsigned int> &Grammar::getTerminals(void)
{
  return terminals;
}

/* **************************************************
 *
 ************************************************** */
std::set<unsigned int> &Grammar::getNonTerminals(void)
{
  return nonTerminals;
}

/* **************************************************
 *
 ************************************************** */
const ruleSet &Grammar::getRules(void) const {
  return rules;
}

/* **************************************************
 *
 ************************************************** */
class Term *Grammar::getStartTerm(void) const {
  return startTerm;
}

/* **************************************************
 *
 ************************************************** */
class Rule *Grammar::getFirstRule(void) const {
  return firstRule;
}

/* **************************************************
 *
 ************************************************** */
const unsigned int *Grammar::getRefIdMax(void) const {
  return &idMax;
}

/* **************************************************
 *
 ************************************************** */
void Grammar::setStartTerm(class Term *startTerm) {
  this->startTerm = startTerm;
}

/* **************************************************
 *
 ************************************************** */
ruleSet::const_iterator Grammar::rulesBegin(void) const {
  return rules.begin();
}

/* **************************************************
 *
 ************************************************** */
ruleSet::const_iterator Grammar::rulesEnd(void) const {
  return rules.end();
}

/* **************************************************
 *
 ************************************************** */
bool Grammar::addRule(class Rule *rule) {
  std::pair<ruleSet::iterator, bool> result = rules.insert(rule);
  return result.second;
}

/* **************************************************
 *
 ************************************************** */
void Grammar::addNewStartTerm(bool addENDTerminal) {
  class Rule *r;
  std::vector<class Terms *> rhs;

  if (addENDTerminal) {
    terminals.insert(Vartable::_END_);
  }

  Vartable::intToStrTable[Vartable::_STARTTERM_] = "_STARTTERM_";
  nonTerminals.insert(Vartable::_STARTTERM_);

  rhs.push_back(new Terms(getStartTerm()));
  if (addENDTerminal) {
    rhs.push_back(new Terms(new class Term(Vartable::_END_)));
  }

  class Term *startTerm = new class Term(Vartable::_STARTTERM_);
  std::string fileName = "";
  r = new Rule(0, fileName, startTerm, rhs);
  setStartTerm(startTerm);
  rules.insert(r);
  firstRule = r;
}

/* **************************************************
 *
 ************************************************** */
void Grammar::print(std::ostream &outStream, class Gitem *gitem) const {
  outStream << "<table><tr><td>";
  outStream << "Terminals={";
  std::set<unsigned int>::const_iterator iter;
  bool first = true;
  for (iter = terminals.begin(); iter != terminals.end(); ++iter) {
    if (first)
      first = false;
    else
      outStream << ", ";
    outStream << Vartable::intToStrTable[*iter] /***<< "(" << *iter << ")"***/;
    
  }
  outStream << "}" << std::endl;
  outStream << "</td></tr><tr><td>";
  outStream << "NonTerminals={";
  first = true;
  for (iter = nonTerminals.begin(); iter != nonTerminals.end(); iter++) {
    if (first)
      first = false;
    else
      outStream << ", ";
    outStream << Vartable::intToStrTable[*iter] /***<< "(" << *iter << ")"***/;
    
  }
  outStream << "}" << std::endl;
  outStream << "</td></tr><tr><td>";
  outStream << "StartTerm=";
  if (startTerm)
    startTerm->print(outStream);
  else
    outStream << "NULL";
  outStream << std::endl;
  outStream << "</td></tr><tr><td>Rules</td></tr>";
  for (ruleSet::const_iterator iterRules = rulesBegin();
       iterRules != rulesEnd(); iterRules++) {
    outStream << "<tr><td>";
    (*iterRules)->print(outStream, -1, true);
    outStream << "</td></tr>";
    
  }
  outStream << "</table>";
  outStream << std::endl;
}

/* **************************************************
 *
 ************************************************** */
void Grammar::addNonTerminal(unsigned int s) {
  nonTerminals.insert(s);
}

/* **************************************************
 *
 ************************************************** */
void Grammar::addTerminal(unsigned int s) {
  terminals.insert(s);
}

/* **************************************************
 *
 ************************************************** */
bool Grammar::isTerminal(class Term *t) const {
  std::set<unsigned int>::const_iterator iter = terminals.find(t->getCode());
  return (iter != terminals.end());
}

/* **************************************************
 *
 ************************************************** */
bool Grammar::isNonTerminal(class Term *t) const {
  std::set<unsigned int>::const_iterator iter = nonTerminals.find(t->getCode());
  return (iter != nonTerminals.end());
}

/* **************************************************
 *
 ************************************************** */
void Grammar::analyseTerms(class Synthesizer &synthesizer) {
  nonTerminals.clear();
  terminals.clear();
  
  ruleSet::const_iterator iterRules;
  for (iterRules = rulesBegin(); iterRules != rulesEnd(); ++iterRules) {
    nonTerminals.insert((*iterRules)->getLhs()->getCode());
  }
  for (iterRules = rulesBegin(); iterRules != rulesEnd(); ++iterRules) {
    unsigned int i;
    for (i = 0; i < (*iterRules)->getRhs().size(); ++i) {
      for (std::vector<class Term *>::const_iterator term = (*iterRules)->getTerms(i)->begin();
	   term != (*iterRules)->getTerms(i)->end();
	   ++term) {
	if (nonTerminals.find((*term)->getCode()) == nonTerminals.end()) {
	  unsigned long int code = (*term)->getCode();
	  terminals.insert(code);
	  
	  std::map<unsigned int, entriesPtr> *predToEntries;
	  std::map<unsigned int, std::map<unsigned int, entriesPtr>*>::iterator foundCode = synthesizer.getLexicon().find(code);
	  if (foundCode == synthesizer.getLexicon().end()) {
	    predToEntries = new std::map<unsigned int, entriesPtr>;
	    predToEntries->insert(std::pair<unsigned int, entriesPtr>(code,
								      Entries::create()));
	    synthesizer.getLexicon().insert(std::pair<unsigned int,
					    std::map<unsigned int, entriesPtr>*>(code, predToEntries));
	  }
	}
      }
    }
  }
}

#ifdef OUTPUT_XML
/* **************************************************
 *
 ************************************************** */
void
Grammar::toXML(xmlNodePtr nodeRoot)
{
  xmlNodePtr g=xmlNewChild(nodeRoot, NULL, (const xmlChar*)"GRAMMAR", NULL);
  xmlNodePtr t=xmlNewChild(g, NULL, (const xmlChar*)"TERMINALS", NULL);
  std::set<unsigned int>::const_iterator iter;
  for (iter=terminals.begin(); iter != terminals.end(); ++iter) {
    xmlNewChild(t, NULL, (const xmlChar*)"TERM", (const xmlChar*)(Vartable::intToStrTable[*iter].c_str()));
  }
  t=xmlNewChild(g, NULL, (const xmlChar*)"NON-TERMINALS", NULL);
  for (iter=nonTerminals.begin(); iter != nonTerminals.end(); ++iter) {
    xmlNewChild(t, NULL, (const xmlChar*)"TERM", (const xmlChar*)(Vartable::intToStrTable[*iter].c_str()));
  }
  xmlNodePtr r=xmlNewChild(g, NULL, (const xmlChar*)"RULES", NULL);
  ruleSet::const_iterator iterRules;
  for (iterRules=rulesBegin(); iterRules != rulesEnd(); ++iterRules) {
    (*iterRules)->toXML(r);
  }
}
#endif

/* **************************************************
 *
 ************************************************** */
std::list<class Rule*> *
Grammar::findRules(class Term *lhs) {
  std::list<class Rule*> *result = new std::list<class Rule*>;
  ruleSet::const_iterator iterRules;
  for (iterRules = rulesBegin(); iterRules != rulesEnd(); iterRules++) {
    if (lhs->getCode() == (*iterRules)->getLhs()->getCode()) {
      result->push_back(*iterRules);

    }
  }
  return result;
}
