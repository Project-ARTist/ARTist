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

#ifndef ART_API_INJECTION_BYTE_H_
#define ART_API_INJECTION_BYTE_H_

#include "primitives.h"

namespace art {

class Byte : public Primitives {
 public:
  Byte()
      : value(0) {}

  explicit Byte(const int32_t _value)
      : value(_value) {}

  int32_t GetValue() const {
    return this->value;
  }

  virtual ~Byte() {}

  virtual ParameterType GetType() const {
    return ParameterType::tByte;
  }
 private:
  int32_t value;
};

}  // namespace art

#endif  // ART_API_INJECTION_BYTE_H_
