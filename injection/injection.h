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

#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include "target.h"
#include "parameter.h"

using std::shared_ptr;
using std::vector;
using std::string;
using std::ostream;

namespace art {

class Injection {
 public:
  Injection(const string& _signature,
            vector<shared_ptr<const Parameter>> _parameter,
            vector<const Target> _injection_target);

  const string ToString() const;

  const string& GetSignature() const;
  const vector<shared_ptr<const Parameter>>& GetParameters() const;
  const vector<const Target>& GetInjectionTargets() const;

 private:
  string signature;

  vector<shared_ptr<const Parameter>> parameters;

  vector<const Target> injection_targets;
};

ostream& operator<<(ostream& os, const Injection& injection);

}  // namespace art

#endif  // ART_INJECTION_INJECTION_H_
