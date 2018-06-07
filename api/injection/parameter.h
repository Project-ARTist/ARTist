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
 * @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
 *
 */

#ifndef ART_API_INJECTION_PARAMETER_H_
#define ART_API_INJECTION_PARAMETER_H_

#include <string>

using std::string;

namespace art {

/**
 * @url https://docs.oracle.com/javase/tutorial/java/nutsandbolts/datatypes.html
 */
enum ParameterType { tParameter = -1,
                     tPrimitive = 0,
                     tBoolean,
                     tByte,
                     tChar,
                     tShort,
                     tInteger,
                     tLong,
                     tFloat,
                     tDouble
};

class Parameter {
 public:
  Parameter() = default;
  Parameter(const Parameter& other) = default;
  Parameter(Parameter&& other) = default;

  virtual ~Parameter() = default;

  Parameter& operator= (const Parameter& other) = default;
  Parameter& operator= (Parameter&& other) = default;

  virtual ParameterType GetType() const = 0;

  virtual const string PrettyName() const;

  static const string TypeToString(const ParameterType& paramType);
};

}  // namespace art

#endif  // ART_API_INJECTION_PARAMETER_H_
