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
 * File:   vpi_user.cpp
 * Author:
 *
 * Created on December 14, 2019, 10:03 PM
 */
#include <uhdm/sv_vpi_user.h>
#include <uhdm/vhpi_user.h>

#include <cctype>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <uhdm/NumUtils.h>
#include <uhdm/Serializer.h>
#include <uhdm/containers.h>
#include <uhdm/uhdm.h>
#include <uhdm/uhdm_types.h>
#include <uhdm/vpi_uhdm.h>

<HEADERS>

using namespace UHDM;

static char* StrClone(std::string_view sv) {
  char* clone = new char[sv.length() + 1];
  std::memcpy(clone, sv.data(), sv.length());
  clone[sv.length()] = '\0';
  return clone;
}

static int32_t StriCmp(std::string_view lhs, std::string_view rhs) {
  for (size_t i = 0, n = std::min(lhs.length(), rhs.length()); i < n; ++i) {
    int32_t lc = std::tolower((int32_t)lhs[i]);
    int32_t rc = std::tolower((int32_t)rhs[i]);
    if (lc != rc) return (lc < rc) ? -1 : +1;
  }

  return (lhs.length() == rhs.length())
    ? 0
    : ((lhs.length() < rhs.length()) ? -1 : +1);
}

UHDM::design* UhdmDesignFromVpiHandle(vpiHandle hdesign) {
  if (!hdesign) return nullptr;
  UHDM::any* tmp = (UHDM::any*)((uhdm_handle*)hdesign)->object;
  if (tmp->UhdmType() == uhdmdesign)
    return (UHDM::design*)tmp;
  else
    return nullptr;
}

s_vpi_value* String2VpiValue(std::string_view sv) {
  while (!sv.empty() && isspace(sv.front())) sv.remove_prefix(1);
  s_vpi_value* val = new s_vpi_value;
  val->format = 0;
  val->value.integer = 0;
  val->value.scalar = 0;
  val->value.str = nullptr;
  if (sv.find("UINT:") == 0) {
    val->format = vpiUIntVal;
    sv.remove_prefix(std::string_view("UINT:").length());
    if (NumUtils::parseUint64(sv, &val->value.uint) == nullptr) {
      val->value.uint = 0;
    }
  } else if (sv.find("INT:") == 0) {
    val->format = vpiIntVal;
    sv.remove_prefix(std::string_view("INT:").length());
    if (NumUtils::parseInt64(sv, &val->value.integer) == nullptr) {
      val->value.integer = 0;
    }
  } else if (sv.find("SCAL:") == 0) {
    val->format = vpiScalarVal;
    sv.remove_prefix(std::string_view("SCAL:").length());
    switch (sv.front()) {
      case 'Z': val->value.scalar = vpiZ; break;
      case 'X': val->value.scalar = vpiX; break;
      case 'H': val->value.scalar = vpiH; break;
      case 'L': val->value.scalar = vpiL; break;
        // Not really clear what the difference between X and DontCare is.
        // Let's parse 'W'eak don't care as this one.
      case 'W': val->value.scalar = vpiDontCare; break;
      default: {
        if (StriCmp(sv, std::string_view("DontCare")) == 0) {
          val->value.scalar = vpiDontCare;
        } else if (StriCmp(sv, std::string_view("NoChange")) == 0) {
          val->value.scalar = vpiNoChange;
        } else {
          // Maybe written numerically?
          if (NumUtils::parseInt32(sv, &val->value.scalar) == nullptr) {
            val->value.scalar = 0;
          }
        }
      } break;
    }
  } else if (sv.find("BIN:") == 0) {
    val->format = vpiBinStrVal;
    sv.remove_prefix(std::string_view("BIN:").length());
    val->value.str = StrClone(sv);
  } else if (sv.find("HEX:") == 0) {
    val->format = vpiHexStrVal;
    sv.remove_prefix(std::string_view("HEX:").length());
    val->value.str = StrClone(sv);
  } else if (sv.find("OCT:") == 0) {
    val->format = vpiOctStrVal;
    sv.remove_prefix(std::string_view("OCT:").length());
    val->value.str = StrClone(sv);
  } else if (sv.find("STRING:") == 0) {
    val->format = vpiStringVal;
    sv.remove_prefix(std::string_view("STRING:").length());
    val->value.str = StrClone(sv);
  } else if (sv.find("REAL:") == 0) {
    val->format = vpiRealVal;
    sv.remove_prefix(std::string_view("REAL:").length());
    if (NumUtils::parseDouble(sv, &val->value.real) == nullptr) {
      val->value.real = 0;
    }
  } else if (sv.find("DEC:") == 0) {
    val->format = vpiDecStrVal;
    sv.remove_prefix(std::string_view("DEC:").length());
    val->value.str = StrClone(sv);
  }

  return val;
}

s_vpi_delay* String2VpiDelays(std::string_view sv) {
  while (!sv.empty() && isspace(sv.front())) sv.remove_prefix(1);
  s_vpi_delay* delay = new s_vpi_delay;
  delay->da = nullptr;
  if (!sv.empty() && (sv.front() == '#')) {
    delay->da = new t_vpi_time;
    delay->no_of_delays = 1;
    delay->time_type = vpiScaledRealTime;
    sv.remove_prefix(1);
    if (NumUtils::parseUint32(sv, &delay->da[0].low) == nullptr) {
      delay->da[0].low = 0;
    }
    delay->da[0].type = vpiScaledRealTime;
  }
  return delay;
}

std::string VpiValue2String(const s_vpi_value* value) {
  static constexpr std::string_view kIntPrefix("INT:");
  static constexpr std::string_view kUIntPrefix("UINT:");
  static constexpr std::string_view kScalPrefix("SCAL:");
  static constexpr std::string_view kStrPrefix("STRING:");
  static constexpr std::string_view kHexPrefix("HEX:");
  static constexpr std::string_view kOctPrefix("OCT:");
  static constexpr std::string_view kBinPrefix("BIN:");
  static constexpr std::string_view kRealPrefix("REAL:");
  static constexpr std::string_view kDecPrefix("DEC:");

  if (!value) return "";
  switch (value->format) {
    case vpiIntVal: return std::string(kIntPrefix).append(std::to_string(value->value.integer));
    case vpiUIntVal: return std::string(kUIntPrefix).append(std::to_string(value->value.uint));
    case vpiScalarVal: {
      switch (value->value.scalar) {
        case vpi0: return "SCAL:0";
        case vpi1: return "SCAL:1";
        case vpiZ: return "SCAL:Z";
        case vpiX: return "SCAL:X";
        case vpiH: return "SCAL:H";
        case vpiL: return "SCAL:L";
        case vpiDontCare: return "SCAL:DontCare";
        case vpiNoChange: return "SCAL:NoChange";
        default:
          // mmh, some unknown number.
          return std::string(kScalPrefix).append(std::to_string(value->value.scalar));
      }
    }
    case vpiStringVal: return std::string(kStrPrefix).append(value->value.str);
    case vpiHexStrVal: return std::string(kHexPrefix).append(value->value.str);
    case vpiOctStrVal: return std::string(kOctPrefix).append(value->value.str);
    case vpiBinStrVal: return std::string(kBinPrefix).append(value->value.str);
    case vpiDecStrVal: return std::string(kDecPrefix).append(value->value.str);
    case vpiRealVal: return std::string(kRealPrefix) .append(std::to_string(value->value.real));
  }

  return "";
}

std::string VpiDelay2String(const s_vpi_delay* delay) {
  std::string result;
  if (delay == nullptr) return result;
  if (delay->da == nullptr) return result;
  switch (delay->time_type) {
    case vpiScaledRealTime: {
      result.append("#").append(std::to_string(delay->da[0].low));
      break;
    }
    default:
      break;
  }
  return result;
}

vpiHandle NewVpiHandle(const UHDM::BaseClass* object) {
  return reinterpret_cast<vpiHandle>(
      new uhdm_handle(object->UhdmType(), object));
}

static vpiHandle NewHandle(UHDM_OBJECT_TYPE type, const void* object) {
  return reinterpret_cast<vpiHandle>(new uhdm_handle(type, object));
}

vpiHandle vpi_handle_by_index(vpiHandle object, PLI_INT32 indx) { return 0; }

vpiHandle vpi_handle_by_name(PLI_BYTE8* name, vpiHandle refHandle) {
  const uhdm_handle* const handle = (const uhdm_handle*)refHandle;
  const BaseClass* const object = (const BaseClass*)handle->object;
  if (object->GetSerializer()->symbolMaker.GetId(name) ==
      SymbolFactory::getBadId()) {
    return nullptr;
  }
  const BaseClass *const ref = object->GetByVpiName(std::string_view(name));
  return (ref != nullptr) ? NewVpiHandle(ref) : nullptr;
}

vpiHandle vpi_handle(PLI_INT32 type, vpiHandle refHandle) {
  const uhdm_handle* const handle = (const uhdm_handle*)refHandle;
  const BaseClass* const object = (const BaseClass*)handle->object;
  auto [ref, ignored1, ignored2] = object->GetByVpiType(type);
  return (ref != nullptr) ? NewHandle(ref->UhdmType(), ref) : nullptr;
}

vpiHandle vpi_handle_multi(PLI_INT32 type, vpiHandle refHandle1,
                           vpiHandle refHandle2, ...) {
  return 0;
}

/* for traversing relationships */

vpiHandle vpi_iterate(PLI_INT32 type, vpiHandle refHandle) {
  const uhdm_handle* const handle = (const uhdm_handle*)refHandle;
  const BaseClass* const object = (const BaseClass*)handle->object;
  auto [ignored, refType, refVector] = object->GetByVpiType(type);
  return (refVector != nullptr) ? NewHandle(refType, refVector) : nullptr;
}

PLI_INT32 vpi_compare_objects(vpiHandle handle1, vpiHandle handle2) {
  const BaseClass* const object1 =
      (const BaseClass*)((const uhdm_handle*)handle1)->object;
  const BaseClass* const object2 =
      (const BaseClass*)((const uhdm_handle*)handle2)->object;
  // NOTE: As per the standard, this API is expected to return a 1 for equal.
  // And, yes that is counter intuitive. But BaseClass::Compare returns a 0
  // for equal. Negate the result here to meet standard requirements.
  return (object1 == object2) ? 1 : ((object1->Compare(object2) == 0) ? 1 : 0);
}

vpiHandle vpi_scan(vpiHandle iterator) {
  if (!iterator) return 0;
  uhdm_handle* handle = (uhdm_handle*)iterator;
  const std::vector<const BaseClass*>* vect =
      (const std::vector<const BaseClass*>*)handle->object;
  if (handle->index < vect->size()) {
    const BaseClass* const object = vect->at(handle->index);
    uhdm_handle* h = new uhdm_handle(object->UhdmType(), object);
    ++handle->index;
    return (vpiHandle) h;
  }
  return nullptr;
}

PLI_INT32 vpi_free_object(vpiHandle object) {
  return vpi_release_handle(object);
}

PLI_INT32 vpi_release_handle(vpiHandle object) {
  delete (uhdm_handle*)object;
  return 0;
}

/* for processing properties */

PLI_INT32 vpi_get(PLI_INT32 property, vpiHandle object) {
  if (!object) {
    std::cout << "VPI ERROR: Bad usage of vpi_get" << std::endl;
    return 0;
  }

  // At this point, the implementation is exactly the same as for 64 bit,
  // but we truncate.
  return (PLI_INT32)vpi_get64(property, object);
}

PLI_INT64 vpi_get64(PLI_INT32 property, vpiHandle object) {
  if (!object) {
    std::cout << "VPI ERROR: Bad usage of vpi_get64" << std::endl;
    return 0;
  }

  const uhdm_handle* const handle = (const uhdm_handle*)object;
  const BaseClass* const obj = (const BaseClass*)handle->object;
  BaseClass::vpi_property_value_t value = obj->GetVpiPropertyValue(property);
  return std::holds_alternative<int64_t>(value) ? std::get<int64_t>(value) : 0;
}

PLI_BYTE8* vpi_get_str(PLI_INT32 property, vpiHandle object) {
  if (!object) {
    std::cout << "VPI ERROR: Bad usage of vpi_get_str" << std::endl;
    return 0;
  }
  const uhdm_handle* const handle = (const uhdm_handle*)object;
  const BaseClass* const obj = (const BaseClass*)handle->object;
  BaseClass::vpi_property_value_t value = obj->GetVpiPropertyValue(property);
  return std::holds_alternative<const char *>(value)
      ? const_cast<char *>(std::get<const char *>(value))
      : nullptr;
}

/* delay processing */

void vpi_get_delays(vpiHandle object, p_vpi_delay delay_p) {
  delay_p->da = nullptr;
  if (!object) {
    std::cout << "VPI ERROR: Bad usage of vpi_get_delay" << std::endl;
    return;
  }
  const uhdm_handle* const handle = (const uhdm_handle*)object;
  const BaseClass* const obj = (const BaseClass*)handle->object;
<VPI_GET_DELAY_BODY>
}

void vpi_put_delays(vpiHandle object, p_vpi_delay delay_p) {}

/* value processing */

void vpi_get_value(vpiHandle vexpr, p_vpi_value value_p) {
  value_p->format = 0;
  if (!vexpr) {
    std::cout << "VPI ERROR: Bad usage of vpi_get_value" << std::endl;
    return;
  }
  const uhdm_handle* const handle = (const uhdm_handle*)vexpr;
  const BaseClass* const obj = (const BaseClass*)handle->object;
<VPI_GET_VALUE_BODY>
}

vpiHandle vpi_put_value(vpiHandle object, p_vpi_value value_p,
                        p_vpi_time time_p, PLI_INT32 flags) {
  return 0;
}

void vpi_get_value_array(vpiHandle object, p_vpi_arrayvalue arrayvalue_p,
                         PLI_INT32* index_p, PLI_UINT32 num) {}

void vpi_put_value_array(vpiHandle object, p_vpi_arrayvalue arrayvalue_p,
                         PLI_INT32* index_p, PLI_UINT32 num) {}

/* time processing */

void vpi_get_time(vpiHandle object, p_vpi_time time_p) {}

PLI_INT32 vpi_get_data(PLI_INT32 id, PLI_BYTE8* dataLoc, PLI_INT32 numOfBytes) {
  return 0;
}

PLI_INT32 vpi_put_data(PLI_INT32 id, PLI_BYTE8* dataLoc, PLI_INT32 numOfBytes) {
  return 0;
}

void* vpi_get_userdata(vpiHandle obj) { return 0; }

PLI_INT32 vpi_put_userdata(vpiHandle obj, void* userdata) { return 0; }

vpiHandle vpi_handle_by_multi_index(vpiHandle obj, PLI_INT32 num_index,
                                    PLI_INT32* index_array) {
  return 0;
}


vpiHandle vpi_register_assertion_cb( vpiHandle assertion, PLI_INT32 reason, vpi_assertion_callback_func *cb_rtn, PLI_BYTE8 *user_data) {
  return 0;
}
