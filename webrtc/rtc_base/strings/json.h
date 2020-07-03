/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_STRINGS_JSON_H_
#define RTC_BASE_STRINGS_JSON_H_

#include <string>
#include <vector>

#if !defined(WEBRTC_EXTERNAL_JSON)
#include "json/json.h"
#else
#include "third_party/jsoncpp/json.h"
#endif

namespace rtc {

///////////////////////////////////////////////////////////////////////////////
// JSON Helpers
///////////////////////////////////////////////////////////////////////////////

// Robust conversion operators, better than the ones in JsonCpp.
bool GetIntFromJson(const SJson::Value& in, int* out);
bool GetUIntFromJson(const SJson::Value& in, unsigned int* out);
bool GetStringFromJson(const SJson::Value& in, std::string* out);
bool GetBoolFromJson(const SJson::Value& in, bool* out);
bool GetDoubleFromJson(const SJson::Value& in, double* out);

// Pull values out of a JSON array.
bool GetValueFromJsonArray(const SJson::Value& in, size_t n, SJson::Value* out);
bool GetIntFromJsonArray(const SJson::Value& in, size_t n, int* out);
bool GetUIntFromJsonArray(const SJson::Value& in, size_t n, unsigned int* out);
bool GetStringFromJsonArray(const SJson::Value& in, size_t n, std::string* out);
bool GetBoolFromJsonArray(const SJson::Value& in, size_t n, bool* out);
bool GetDoubleFromJsonArray(const SJson::Value& in, size_t n, double* out);

// Convert json arrays to std::vector
bool JsonArrayToValueVector(const SJson::Value& in,
                            std::vector<SJson::Value>* out);
bool JsonArrayToIntVector(const SJson::Value& in, std::vector<int>* out);
bool JsonArrayToUIntVector(const SJson::Value& in,
                           std::vector<unsigned int>* out);
bool JsonArrayToStringVector(const SJson::Value& in,
                             std::vector<std::string>* out);
bool JsonArrayToBoolVector(const SJson::Value& in, std::vector<bool>* out);
bool JsonArrayToDoubleVector(const SJson::Value& in, std::vector<double>* out);

// Convert std::vector to json array
SJson::Value ValueVectorToJsonArray(const std::vector<SJson::Value>& in);
SJson::Value IntVectorToJsonArray(const std::vector<int>& in);
SJson::Value UIntVectorToJsonArray(const std::vector<unsigned int>& in);
SJson::Value StringVectorToJsonArray(const std::vector<std::string>& in);
SJson::Value BoolVectorToJsonArray(const std::vector<bool>& in);
SJson::Value DoubleVectorToJsonArray(const std::vector<double>& in);

// Pull values out of a JSON object.
bool GetValueFromJsonObject(const SJson::Value& in,
                            const std::string& k,
                            SJson::Value* out);
bool GetIntFromJsonObject(const SJson::Value& in,
                          const std::string& k,
                          int* out);
bool GetUIntFromJsonObject(const SJson::Value& in,
                           const std::string& k,
                           unsigned int* out);
bool GetStringFromJsonObject(const SJson::Value& in,
                             const std::string& k,
                             std::string* out);
bool GetBoolFromJsonObject(const SJson::Value& in,
                           const std::string& k,
                           bool* out);
bool GetDoubleFromJsonObject(const SJson::Value& in,
                             const std::string& k,
                             double* out);

// Writes out a Json value as a string.
std::string JsonValueToString(const SJson::Value& json);

}  // namespace rtc

#endif  // RTC_BASE_STRINGS_JSON_H_
