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

#include "injection.h"
#include "parameter.h"
#include <iostream>

using std::move;

namespace art {
// we need a `shared_ptr` for `Parameter` since it is an abstract class and hence we cannot create `vector<Parameter>`.
Injection::Injection(const string &_signature,
                     vector<shared_ptr<const Parameter>> _parameter,
                     vector<shared_ptr<const Target>> _injection_target)
    : signature(_signature)
    , parameters(_parameter)
    , injection_targets(_injection_target) {}

const string Injection::ToString() const {
  string string_ = "Injection: ";

  string_ += (this->signature + " [WHAT] \n");
  for (auto && param : this->parameters) {
    string_ += ("> P: " + param->PrettyName() + ", \n");
  }
  string_ = (string_.substr(0, string_.size() - 3) + "\n");

  for (auto && target : injection_targets) {
    string_ += (">> T: " + target->GetTargetSignature() + "\n");
  }
  return string_;
}


const string& Injection::GetSignature() const {
  return signature;
}

const vector<shared_ptr<const Parameter>>& Injection::GetParameters() const {
  return parameters;
}

const vector<shared_ptr<const Target>>& Injection::GetInjectionTargets() const {
  return injection_targets;
}

std::ostream& operator<<(std::ostream& os, const Injection& injection) {
  os << injection.ToString();
  return os;
}

}  // namespace art
