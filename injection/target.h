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

#ifndef ART_INJECTION_TARGET_H_
#define ART_INJECTION_TARGET_H_

#include <string>

using std::string;

namespace art {

enum InjectionTarget { NO_INJECTION = -1
                     , METHOD_START = 0
                     , METHOD_END
                     , METHOD_CALL_BEFORE
                     , METHOD_CALL_AFTER };

/** Defines where the MethodCall should get injected
 *
 * Currently:
 *
 * - Start of Method
 * - End of MethodCall
 * - Before MethodCall
 * - After Method
 *
 * Those are the most important use cases for the first Release.
 * Proper Modular Implementation could/should use subclassing
 * of @see Target.
 *
 */
class Target {
 public:
  Target(const string& targetSignature, InjectionTarget targetType);
  Target(const Target& copy);
  virtual ~Target();

  Target& operator= (const Target& other);

  const string GetTargetSignature() const;

  InjectionTarget GetTargetType() const;

 public:
  static const char GENERIC_TARGET[];

 private:
  string target_signature;
  InjectionTarget target_type;
};

}  // namespace art

#endif  // ART_INJECTION_TARGET_H_
