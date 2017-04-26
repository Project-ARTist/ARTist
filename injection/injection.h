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

#ifndef ART_INJECTION_INJECTION_H_
#define ART_INJECTION_INJECTION_H_

#include <string>
#include <vector>
#include <unordered_set>
#include "target.h"
#include "parameter.h"

namespace art {

class Injection {
 public:
  Injection();
  Injection(const std::string& _signature,
            const std::vector<std::shared_ptr<Parameter>>& _parameter,
            const Target& _injection_target);

  Injection(const std::string& _signature,
            const std::vector<std::shared_ptr<Parameter>>& _parameter,
            const std::vector<Target>& _targets);

  Injection(const Injection& copy);

  virtual ~Injection() {}

  Injection& operator= (const Injection&);

  const std::string ToString() const;

  const std::string GetSignature() const;
  const std::vector<std::shared_ptr<Parameter>>& GetParameters() const;
  const std::vector<Target>& GetInjectionTargets() const;

 private:
  std::string signature;

  std::vector<std::shared_ptr<Parameter>> parameters;

  std::vector<Target> injection_targets;

  friend std::ostream& operator<<(std::ostream& os, const Injection& injection);
};

std::ostream& operator<<(std::ostream& os, const Injection& injection);

}  // namespace art

#endif  // ART_INJECTION_INJECTION_H_
