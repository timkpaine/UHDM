/*
 Do not modify, auto-generated by model_gen.tcl

 Copyright 2019 Alain Dargelas

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
 * File:   Serializer.cpp
 * Author:
 *
 * Created on October 4, 2021, 10:53 PM
 */
#include <uhdm/Serializer.h>
#include <uhdm/UhdmListener.h>
#include <uhdm/vpi_visitor.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <uhdm/uhdm.h>

namespace UHDM {

const uint32_t Serializer::kVersion = 1;

void Serializer::GarbageCollect() {
  if (!m_enableGC) return;

  UhdmListener* const listener = new UhdmListener();
  for (auto d : designMaker.objects_) {
    listener->listenDesign(d);
  }

  const AnySet visited(listener->getVisited().begin(), listener->getVisited().end());
  delete listener;

<FACTORY_GC>
}

void DefaultErrorHandler(ErrorType errType, const std::string& errorMsg, const any* object1, const any* object2) {
  std::cerr << errorMsg << std::endl;
}

SymbolId Serializer::MakeSymbol(std::string_view symbol) {
  return symbolMaker.Make(symbol);
}

std::string_view Serializer::GetSymbol(SymbolId id) const {
  return symbolMaker.GetSymbol(id);
}

SymbolId Serializer::GetSymbolId(std::string_view symbol) const {
  return symbolMaker.GetId(symbol);
}

vpiHandle Serializer::MakeUhdmHandle(UHDM_OBJECT_TYPE type, const void* object) {
  return uhdm_handleMaker.Make(type, object);
}

Serializer::IdMap Serializer::AllObjects() const {
  IdMap idMap;
<CAPNP_ID>
  return idMap;
}

std::string UhdmName(UHDM_OBJECT_TYPE type) {
  switch (type) {
<UHDM_NAME_MAP>
    default: return "NO TYPE";
  }
}

// From uhdm_types.h
std::string VpiTypeName(vpiHandle h) {
  uhdm_handle* handle = (uhdm_handle*)h;
  BaseClass* obj = (BaseClass*)handle->object;
  return UhdmName(obj->UhdmType());
}

std::map<std::string, uint32_t, std::less<>> Serializer::ObjectStats() const {
  std::map<std::string, uint32_t, std::less<>> stats;
<FACTORY_STATS>
  return stats;
}

void Serializer::PrintStats(std::ostream& strm,
                            std::string_view infoText) const {
  strm << "=== UHDM Object Stats Begin (" << infoText << ") ===" << std::endl;
  auto stats = ObjectStats();
  std::vector<std::string_view> names;
  names.reserve(stats.size());
  std::transform(stats.begin(), stats.end(), std::back_inserter(names),
                 [](decltype(stats)::value_type const& pair) {
                   return std::string_view(pair.first);
                 });
  std::sort(names.begin(), names.end());
  for (std::string_view name : names) {
    auto it = stats.find(name);
    if (it->second > 0) {
      // The longest model name is
      // "enum_struct_union_packed_array_typespec_group"
      strm << std::setw(48) << std::left << name << std::setw(8) << std::right
           << it->second << std::endl;
    }
  }
  strm << "=== UHDM Object Stats End ===" << std::endl;
}

bool Serializer::Erase(const BaseClass* p) {
  if (p == nullptr) {
    return true;
  }

  switch (p->UhdmType()) {
<FACTORY_ERASE_OBJECT>
    default: return false;
  }
}

Serializer::~Serializer() {
  Purge();
}

void Serializer::Purge() {
  anyVectMaker.Purge();
  symbolMaker.Purge();
  uhdm_handleMaker.Purge();
<FACTORY_PURGE>
}
}  // namespace UHDM
