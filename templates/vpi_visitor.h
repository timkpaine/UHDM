/*
 Do not modify, auto-generated by model_gen.tcl

 Copyright 2019-2020 Alain Dargelas

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
 * File:   vpi_visitor.h
 * Author:
 *
 * Created on December 14, 2019, 10:03 PM
 */

#include <string>
#include <vector>
#include <set>
#include "sv_vpi_user.h"


#ifndef UHDM_VPI_VISITOR_H
#define UHDM_VPI_VISITOR_H

namespace UHDM {
#ifdef STANDARD_VPI
typedef std::set<vpiHandle> VisitedContainer;
#else
typedef  std::set<const UHDM::BaseClass*> VisitedContainer;
#endif
  
// Visit an object, dump to given stream. 
void visit_object (vpiHandle obj_h, int indent, const char *relation, VisitedContainer* visited, std::ostream& out, bool shallowVisit = false); 

// Visit designs, dump to given stream.
void visit_designs (const std::vector<vpiHandle>& designs, std::ostream &out);

// Visit designs, return string representation.
std::string visit_designs (const std::vector<vpiHandle>& designs);

};

#endif
