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

#ifndef ART_INJECTION_VISITOR_KEYS_H_
#define ART_INJECTION_VISITOR_KEYS_H_

#include <string>

namespace art {

class VisitorKeys {
 public:
  static const std::string H_INVOKE;
  static const std::string H_INVOKE_INTERFACE;
  static const std::string H_INVOKE_STATIC_OR_DIRECT;
  static const std::string H_INVOKE_VIRTUAL;

  static const std::string H_RETURN;
  static const std::string H_RETURN_VOID;
};

}  // namespace art

#endif  // ART_INJECTION_VISITOR_KEYS_H_
