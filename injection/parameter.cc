/**
 * The ARTist Project (https://artist.cispa.saarland)
 *
 * Copyright (C) 2017 CISPA (https://cispa.saarland), Saarland University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @author "Oliver Schranz <oliver.schranz@cispa.saarland>"
 * @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
 *
 */

#include "parameter.h"
#include "optimizing/artist/artist_log.h"

namespace art {

Parameter::Parameter() {
}

Parameter::Parameter(const Parameter& copy) {
  DCHECK(&copy);
}

Parameter::~Parameter() {
}

Parameter& Parameter::operator= (const Parameter& other) {
  // check for self-assignment
  if (&other == this) {
    return *this;
  }
  return *this;
}

const std::string Parameter::PrettyName() const {
  return TypeToString(this->GetType());
}

const std::string Parameter::TypeToString(const ParameterType& paramType) {
  switch (paramType) {
    case ParameterType::tParameter:
      return "ParameterType::tParameter";
    case ParameterType::tPrimitive:
      return "ParameterType::tPrimitive";
    case ParameterType::tBoolean:
      return "ParameterType::tBoolean";
    case ParameterType::tByte:
      return "ParameterType::tByte";
    case ParameterType::tChar:
      return "ParameterType::tChar";
    case ParameterType::tShort:
      return "ParameterType::tShort";
    case ParameterType::tInteger:
      return "ParameterType::tInteger";
    case ParameterType::tLong:
      return "ParameterType::tLong";
    case ParameterType::tFloat:
      return "ParameterType::tFloat";
    case ParameterType::tDouble:
      return "ParameterType::tDouble";
    case ParameterType::tObject:
      return "ParameterType::tObject";
    case ParameterType::tString:
      return "ParameterType::tString";
    default:
      return "ParameterType: NOT Found: ";
  }
}

}  // namespace art
