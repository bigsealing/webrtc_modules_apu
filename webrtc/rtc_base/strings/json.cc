/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "rtc_base/strings/json.h"

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "rtc_base/string_encode.h"

namespace rtc {

bool GetStringFromJson(const SJson::Value& in, std::string* out) {
  if (!in.isString()) {
    if (in.isBool()) {
      *out = rtc::ToString(in.asBool());
    } else if (in.isInt()) {
      *out = rtc::ToString(in.asInt());
    } else if (in.isUInt()) {
      *out = rtc::ToString(in.asUInt());
    } else if (in.isDouble()) {
      *out = rtc::ToString(in.asDouble());
    } else {
      return false;
    }
  } else {
    *out = in.asString();
  }
  return true;
}

bool GetIntFromJson(const SJson::Value& in, int* out) {
  bool ret;
  if (!in.isString()) {
    ret = in.isConvertibleTo(SJson::intValue);
    if (ret) {
      *out = in.asInt();
    }
  } else {
    long val;  // NOLINT
    const char* c_str = in.asCString();
    char* end_ptr;
    errno = 0;
    val = strtol(c_str, &end_ptr, 10);  // NOLINT
    ret = (end_ptr != c_str && *end_ptr == '\0' && !errno && val >= INT_MIN &&
           val <= INT_MAX);
    *out = val;
  }
  return ret;
}

bool GetUIntFromJson(const SJson::Value& in, unsigned int* out) {
  bool ret;
  if (!in.isString()) {
    ret = in.isConvertibleTo(SJson::uintValue);
    if (ret) {
      *out = in.asUInt();
    }
  } else {
    unsigned long val;  // NOLINT
    const char* c_str = in.asCString();
    char* end_ptr;
    errno = 0;
    val = strtoul(c_str, &end_ptr, 10);  // NOLINT
    ret = (end_ptr != c_str && *end_ptr == '\0' && !errno && val <= UINT_MAX);
    *out = val;
  }
  return ret;
}

bool GetBoolFromJson(const SJson::Value& in, bool* out) {
  bool ret;
  if (!in.isString()) {
    ret = in.isConvertibleTo(SJson::booleanValue);
    if (ret) {
      *out = in.asBool();
    }
  } else {
    if (in.asString() == "true") {
      *out = true;
      ret = true;
    } else if (in.asString() == "false") {
      *out = false;
      ret = true;
    } else {
      ret = false;
    }
  }
  return ret;
}

bool GetDoubleFromJson(const SJson::Value& in, double* out) {
  bool ret;
  if (!in.isString()) {
    ret = in.isConvertibleTo(SJson::realValue);
    if (ret) {
      *out = in.asDouble();
    }
  } else {
    double val;
    const char* c_str = in.asCString();
    char* end_ptr;
    errno = 0;
    val = strtod(c_str, &end_ptr);
    ret = (end_ptr != c_str && *end_ptr == '\0' && !errno);
    *out = val;
  }
  return ret;
}

namespace {
template <typename T>
bool JsonArrayToVector(const SJson::Value& value,
                       bool (*getter)(const SJson::Value& in, T* out),
                       std::vector<T>* vec) {
  vec->clear();
  if (!value.isArray()) {
    return false;
  }

  for (SJson::Value::ArrayIndex i = 0; i < value.size(); ++i) {
    T val;
    if (!getter(value[i], &val)) {
      return false;
    }
    vec->push_back(val);
  }

  return true;
}
// Trivial getter helper
bool GetValueFromJson(const SJson::Value& in, SJson::Value* out) {
  *out = in;
  return true;
}
}  // unnamed namespace

bool JsonArrayToValueVector(const SJson::Value& in,
                            std::vector<SJson::Value>* out) {
  return JsonArrayToVector(in, GetValueFromJson, out);
}

bool JsonArrayToIntVector(const SJson::Value& in, std::vector<int>* out) {
  return JsonArrayToVector(in, GetIntFromJson, out);
}

bool JsonArrayToUIntVector(const SJson::Value& in,
                           std::vector<unsigned int>* out) {
  return JsonArrayToVector(in, GetUIntFromJson, out);
}

bool JsonArrayToStringVector(const SJson::Value& in,
                             std::vector<std::string>* out) {
  return JsonArrayToVector(in, GetStringFromJson, out);
}

bool JsonArrayToBoolVector(const SJson::Value& in, std::vector<bool>* out) {
  return JsonArrayToVector(in, GetBoolFromJson, out);
}

bool JsonArrayToDoubleVector(const SJson::Value& in, std::vector<double>* out) {
  return JsonArrayToVector(in, GetDoubleFromJson, out);
}

namespace {
template <typename T>
SJson::Value VectorToJsonArray(const std::vector<T>& vec) {
  SJson::Value result(SJson::arrayValue);
  for (size_t i = 0; i < vec.size(); ++i) {
    result.append(SJson::Value(vec[i]));
  }
  return result;
}
}  // unnamed namespace

SJson::Value ValueVectorToJsonArray(const std::vector<SJson::Value>& in) {
  return VectorToJsonArray(in);
}

SJson::Value IntVectorToJsonArray(const std::vector<int>& in) {
  return VectorToJsonArray(in);
}

SJson::Value UIntVectorToJsonArray(const std::vector<unsigned int>& in) {
  return VectorToJsonArray(in);
}

SJson::Value StringVectorToJsonArray(const std::vector<std::string>& in) {
  return VectorToJsonArray(in);
}

SJson::Value BoolVectorToJsonArray(const std::vector<bool>& in) {
  return VectorToJsonArray(in);
}

SJson::Value DoubleVectorToJsonArray(const std::vector<double>& in) {
  return VectorToJsonArray(in);
}

bool GetValueFromJsonArray(const SJson::Value& in, size_t n, SJson::Value* out) {
  if (!in.isArray() || !in.isValidIndex(static_cast<int>(n))) {
    return false;
  }

  *out = in[static_cast<SJson::Value::ArrayIndex>(n)];
  return true;
}

bool GetIntFromJsonArray(const SJson::Value& in, size_t n, int* out) {
  SJson::Value x;
  return GetValueFromJsonArray(in, n, &x) && GetIntFromJson(x, out);
}

bool GetUIntFromJsonArray(const SJson::Value& in, size_t n, unsigned int* out) {
  SJson::Value x;
  return GetValueFromJsonArray(in, n, &x) && GetUIntFromJson(x, out);
}

bool GetStringFromJsonArray(const SJson::Value& in, size_t n, std::string* out) {
  SJson::Value x;
  return GetValueFromJsonArray(in, n, &x) && GetStringFromJson(x, out);
}

bool GetBoolFromJsonArray(const SJson::Value& in, size_t n, bool* out) {
  SJson::Value x;
  return GetValueFromJsonArray(in, n, &x) && GetBoolFromJson(x, out);
}

bool GetDoubleFromJsonArray(const SJson::Value& in, size_t n, double* out) {
  SJson::Value x;
  return GetValueFromJsonArray(in, n, &x) && GetDoubleFromJson(x, out);
}

bool GetValueFromJsonObject(const SJson::Value& in,
                            const std::string& k,
                            SJson::Value* out) {
  if (!in.isObject() || !in.isMember(k)) {
    return false;
  }

  *out = in[k];
  return true;
}

bool GetIntFromJsonObject(const SJson::Value& in,
                          const std::string& k,
                          int* out) {
  SJson::Value x;
  return GetValueFromJsonObject(in, k, &x) && GetIntFromJson(x, out);
}

bool GetUIntFromJsonObject(const SJson::Value& in,
                           const std::string& k,
                           unsigned int* out) {
  SJson::Value x;
  return GetValueFromJsonObject(in, k, &x) && GetUIntFromJson(x, out);
}

bool GetStringFromJsonObject(const SJson::Value& in,
                             const std::string& k,
                             std::string* out) {
  SJson::Value x;
  return GetValueFromJsonObject(in, k, &x) && GetStringFromJson(x, out);
}

bool GetBoolFromJsonObject(const SJson::Value& in,
                           const std::string& k,
                           bool* out) {
  SJson::Value x;
  return GetValueFromJsonObject(in, k, &x) && GetBoolFromJson(x, out);
}

bool GetDoubleFromJsonObject(const SJson::Value& in,
                             const std::string& k,
                             double* out) {
  SJson::Value x;
  return GetValueFromJsonObject(in, k, &x) && GetDoubleFromJson(x, out);
}

std::string JsonValueToString(const SJson::Value& json) {
  SJson::FastWriter w;
  std::string value = w.write(json);
  return value.substr(0, value.size() - 1);  // trim trailing newline
}

}  // namespace rtc
