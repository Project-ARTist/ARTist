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

namespace art {

Injection::Injection()
    : signature()
    , parameters()
    , injection_targets() { }

Injection::Injection(const std::string& _signature,
                     const std::vector<std::shared_ptr<Parameter>>& _parameter,
                     const std::vector<Target>& _targets)
    : signature(_signature)
    , parameters(_parameter)
    , injection_targets(_targets) { }

Injection::Injection(const std::string& _signature,
                     const std::vector<std::shared_ptr<Parameter>>& _parameter,
                     const Target& _injection_target)
    : signature(_signature)
    , parameters(_parameter)
    , injection_targets() {
    injection_targets.push_back(_injection_target);
}

Injection::Injection(const Injection& copy)
    : signature(copy.GetSignature())
    , parameters(copy.GetParameters())
    , injection_targets(copy.GetInjectionTargets()) { }

Injection& Injection::operator= (const Injection& other) {
  // check for self-assignment
  if (&other == this) {
    return *this;
  }
  signature = other.GetSignature();
  parameters = other.GetParameters();
  injection_targets = other.GetInjectionTargets();
  return *this;
}

const std::string Injection::ToString() const {
  std::string string_ = "Injection: ";

  string_ += (this->signature + " [WHAT] \n");
  for (auto && param : this->parameters) {
    string_ += ("> P: " + param->PrettyName() + ", \n");
  }
  string_ = (string_.substr(0, string_.size() - 3) + "\n");

  for (auto && target : injection_targets) {
    string_ += (">> T: " + target.GetTargetSignature() + "\n");
  }
  return string_;
}


const std::string Injection::GetSignature() const {
  return signature;
}

const std::vector<std::shared_ptr<Parameter>>& Injection::GetParameters() const {
  return parameters;
}

const std::vector<Target>& Injection::GetInjectionTargets() const {
  return injection_targets;
}

std::ostream& operator<<(std::ostream& os, const Injection& injection) {
  os << injection.ToString();
  return os;
}

}  // namespace art
