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

#ifndef ART_INJECTION_PARAMETER_H_
#define ART_INJECTION_PARAMETER_H_

#include <string>

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
                     tDouble,
                     tObject,
                     tString
};

class Parameter {
 public:
  Parameter();
  Parameter(const Parameter& copy);

  virtual ~Parameter();

  Parameter& operator= (const Parameter& other);

  virtual ParameterType GetType() const = 0;

  virtual const std::string PrettyName() const;

  static const std::string TypeToString(const ParameterType& paramType);
};

}  // namespace art

#endif  // ART_INJECTION_PARAMETER_H_
