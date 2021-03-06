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

#include <regex>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include "environment.hh"
#include "messages.hh"
#include "list.hh"
#include "feature.hh"
#include "features.hh"
#include "value.hh"
#include "ipointer.hh"
#include "listfeatures.hh"

/* **************************************************
 *
 ************************************************** */
Environment::Environment()
{
  NEW;
}

/* **************************************************
 *
 ************************************************** */
Environment::~Environment()
{
  for (std::map<const std::string, valuePtr>::iterator i = env.begin();
       i != env.end();
       ++i) {
    valuePtr tmp = i->second;
    if (tmp)
      tmp.reset();
  }
  DELETE;    
}

/* **************************************************
 *
 ************************************************** */
environmentPtr Environment::create()
{
  return environmentPtr( new Environment() );
}

/* **************************************************
 *
 ************************************************** */
void Environment::add(std::string a, valuePtr b)
{
  Environment::mapStringValue::iterator i=env.find(a);
  if (i==env.end()){
    env.insert (std::pair <std::string, valuePtr >(a, b));
  }
  else{
    i->second = b;
  }
}

 /* **************************************************
  *
  ************************************************** */
 void Environment::add(bitsetPtr a, valuePtr b)
 {
   std::string k = a->toString();
   add (k, b);
 }

/* **************************************************
 *
 ************************************************** */
void Environment::add(environmentPtr e)
{
  for (Environment::mapStringValue::const_iterator i=e->begin(); i!=e->end(); i++)
    add(i->first, i->second);
}

/* **************************************************
 *
 ************************************************** */
void Environment::add(environmentPtr e, environmentPtr where)
{
  if (where)
    for (Environment::mapStringValue::const_iterator i=e->begin(); i!=e->end(); i++)
      if (where->env.find(i->first)!=where->env.end())
	add(i->first, i->second);
}

/* **************************************************
 *
 ************************************************** */
Environment::mapStringValue::const_iterator Environment::begin() const 
{
  return env.begin();
}

/* **************************************************
 *
 ************************************************** */
Environment::mapStringValue::const_iterator Environment::end() const 
{
  return env.end();
}

/* **************************************************
 *
 ************************************************** */
const size_t Environment::size() const 
{
  return env.size();
}

/* **************************************************
 *
 ************************************************** */
valuePtr 
Environment::find(bitsetPtr variable) const 
{
  valuePtr ret = valuePtr();
  /*** 
  cerr << "findValue: " << variable->toString() << std::endl;
  cerr << std::endl;
   ***/
  Environment::mapStringValue::const_iterator it=env.find(variable->toString());
  if (it != env.end())
    ret = it->second;
  /*** 
  if (ret)
    ret->print(cerr);
  else 
    cerr << "NULL";
  cerr << std::endl;
   ***/
  return ret;
}

/* **************************************************
 *
 ************************************************** */
void
Environment::print(std::ostream &out) const 
{
  bool first=true;
  Environment::mapStringValue::const_iterator i=begin();
  out << "<TABLE border=\"0\"><TR>";
  while(i!=end()){
    if (first)
      first=false;
    else
      out << "</TR><TR>";
    out << "<TD>";
    //out << Bitset::bitsToStrTable[(*i).first];
    out << (*i).first;
    if ((*i).second){
      out << ":</TD><TD>";
      (*i).second->print(out);
    }
    out << "</TD>";
    i++;
  }
  out << "</TR></TABLE>";
}

/* **************************************************
 *
 ************************************************** */
void
Environment::replaceVariables(featuresPtr features, bool &effect)
{
  /***
      std::cerr << "<H4>Environment::replaceVariables(features)</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>featurePtrs</th><th>Environment</th></tr>";
      std::cerr << "<tr><td>";
      features->print(std::cerr);
      std::cerr << "</td><td>";
      this->print(std::cerr);
      std::cerr << "</td></tr></table>";
       ***/
  //features->resetSerialId();
 redo:
  for (std::list< featurePtr >::iterator feature = features->begin();
       feature != features->end();
       ++feature){
    switch ((*feature)->getType()){
    case Feature::PRED:
      if ((*feature)->getValue())
	(*feature)->setValue(replaceVariables((*feature)->getValue(), effect));
      break;
    case Feature::FORM:
    case Feature::CONSTANT:
      if ((*feature)->getValue()){
	valuePtr v = replaceVariables((*feature)->getValue(), effect);
	if (v)
	  (*feature)->setValue(v);
      }
      break;
    case Feature::VARIABLE:{
      valuePtr value=find((*feature)->getAttribute());
      if ((*feature)->getValue()){
	effect = true;
	valuePtr v = replaceVariables((*feature)->getValue(), effect);
	if (v)
	  (*feature)->setValue(v);
      }

      if (!value){
	/* do nothing */
      }

      else if (value->isNil()){
	/* do nothing */
      }

      else if (value->isFeatures()){
	features->erase(feature);
	for (std::list< featurePtr >::iterator f = value->getFeatures()->begin();
	     f != value->getFeatures()->end();
	     ++f){
	  features->addFeature(*f);
	}
	effect = true;
	goto redo;
      }

      else if (value->isConstant()){
	(*feature)->setType(Feature::CONSTANT);
	(*feature)->setAttribute(value->getBits());
      }

      else if (value->isAnonymous()){
	features->erase(feature);
	effect = true;
	goto redo;
      }

      else {
	FATAL_ERROR_MSG("environment: variable substitution failed");
      }
    }
      break;
    }
  }
  /***
      std::cerr << "<H4>Environment::replaceVariables(features) DONE</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>Features</th><th>Environment</th></tr>";
      std::cerr << "<tr><td>";
      features->print(std::cerr);
      std::cerr << "</td><td>";
      this->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  if (effect)
    features->resetSerialId();
}

/* **************************************************
 *
 ************************************************** */
void
Environment::replaceVariables(listFeaturesPtr vfeatures, bool &effect)
{
  for (std::vector< featuresPtr >::const_iterator vf = vfeatures->begin();
       vf != vfeatures->end();
       ++vf)
    if (*vf)
      this->replaceVariables(*vf, effect);
}

/* **************************************************
 *
 ************************************************** */
valuePtr 
Environment::replaceVariables(valuePtr value, bool &effect)
{
  valuePtr ret=value;
  /***
      std::cerr << "<H4>Environment::replaceVariables(value)</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>Value</th><th>Environment</th></tr>";
      std::cerr << "<tr><td>";
      value->print(std::cerr);
      std::cerr << "</td><td>";
      this->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  if (!value->isNil() && !value->isTrue()){
    switch(value->getType()){
    case Value::BOOL:
    case Value::STR:
    case Value::CONSTANT:
    case Value::IDENTIFIER:
    case Value::ANONYMOUS:
    case Value::DOUBLE:
      break;
    case Value::FEATURES:
      replaceVariables(value->getFeatures(), effect);
      break;
    case Value::LIST:
      replaceVariables(value->getList(), effect);
      break;
    case Value::VARIABLE:
      valuePtr val = find(value->getBits());
      if (val && (val != value)){
	effect = true;
	ret = replaceVariables(val, effect);
      }
      else
	ret = value;
      break;
    }
  }
  /***
      std::cerr << "<H4>Environment::replaceVariables(value) result</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>Std::List</th></tr>";
      std::cerr << "<tr><td>";
      ret->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  if (effect)
    value->resetSerialId();
  return ret;
}

/* **************************************************
 *
 ************************************************** */
void
Environment::replaceVariables(listPtr list, bool &effect)
{
  /***
      std::cerr << "<H4>Environment::replaceVariables(list)</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>Std::List</th><th>Environment</th></tr>";
      std::cerr << "<tr><td>";
      list->print(std::cerr);
      std::cerr << "</td><td>";
      this->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  //if (list->isSetFlags(SEEN_FLAG))
  //return;
  //list->addFlags(SEEN_FLAG);
  switch (list->getType()){
  case List::ATOM:
    list->setValue(replaceVariables(list->getValue(), effect));
    break;
  case List::PAIRP:
    replaceVariables(list->car(), effect);
    replaceVariables(list->cdr(), effect);
    break;
  case List::NIL:
    break;
  }
  /***
      std::cerr << "<H4>Environment::replaceVariables(list) result</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>Std::List</th></tr>";
      std::cerr << "<tr><td>";
      list->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  if (effect)
    list->resetSerialId();
}

/* **************************************************
 *
 ************************************************** */
std::string
Environment::replaceVariables(std::string str, bool &effect)
{
  /***
      std::cerr << "<H4>Environment::replaceVariables(list)</H4>" << std::endl;
      std::cerr << "<table border=\"1\"><tr><th>std::string</th><th>Environment</th></tr>";
      std::cerr << "<tr><td>";
      std::cerr << str;
      std::cerr << "</td><td>";
      this->print(std::cerr);
      std::cerr << "</td></tr></table>";
  ***/
  std::string result = str;
  std::string pattern = std::string("(\\$([a-zA-Z_]|à|á|â|ã|ä|å|æ|ç|è|é|ê|ë|ì|í|î|ï|ð|ñ|ò|ó|ô|õ|ö|ø|ù|ú|û|ü|ý|ÿ|À|Á|Â|Ã|Ä|Å|Æ|Ç|È|É|Ë|Ì|Í|Î|Ï|Ð|Ñ|Ò|Ó|Ô|Õ|Ö|Ø|Ù|Ú|Û|Ü|Ý|Ÿ|ß)([a-zA-Z0-9_]|à|á|â|ã|ä|å|æ|ç|è|é|ê|ë|ì|í|î|ï|ð|ñ|ò|ó|ô|õ|ö|ø|ù|ú|û|ü|ý|ÿ|À|Á|Â|Ã|Ä|Å|Æ|Ç|È|É|Ë|Ì|Í|Î|Ï|Ð|Ñ|Ò|Ó|Ô|Õ|Ö|Ø|Ù|Ú|Û|Ü|Ý|Ÿ|ß)+)");
  std::cmatch match;
  
  try {
    std::regex regexpression(pattern, std::regex_constants::ECMAScript);   
    while (std::regex_search(result.c_str(), match, regexpression, std::regex_constants::format_first_only)){
      std::string s = match[1];
      Environment::mapStringValue::iterator i = this->env.find(s);
      if (i == this->env.end()){
	std::cerr << "*** error variable " << s << " not found" << std::endl;
	exit(1);
      }
      else{
	std::ostringstream oss;
	i->second->print(oss);
	result = std::regex_replace(result, regexpression, oss.str(), std::regex_constants::format_first_only);
      }
    }
  }
  
  catch (const std::regex_error& e) {
    std::cout << "regex_error caught: " << e.what() << '(' << e.code() << ')' << std::regex_constants::error_brack << '\n';
  }
  /*** 
       std::cerr << "<H4>Environment::replaceVariables(list) result</H4>" << std::endl;
       std::cerr << "<table border=\"1\"><tr><th>Std::List</th></tr>";
       std::cerr << "<tr><td>";
       std::cerr << result;
       std::cerr << "</td></tr></table>";
  ***/
  return result;
}

/* **************************************************
 *
 ************************************************** */
environmentPtr
Environment::clone(void)
{
  environmentPtr environment=Environment::create();
  for (Environment::mapStringValue::const_iterator i = begin();
       i != end();
       ++i){
    environment->add((*i).first,
		     ((*i).second) ? (*i).second->clone() : valuePtr());
  }
  return environment;
}
