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
 *
 */

#include "method_name_filters.h"

namespace art {

// depending on _accept this implementation either works like a whitelist (true) or blacklist (false) and ca be
// fine-tuned to use exact matches and full signatures.
bool MethodNameFilter::accept(const art::MethodInfo &info) const {
  auto candidate = info.GetMethodName(_signature);
  for (auto name : _names) {
    auto pos = candidate.find(name);
    // condition: match && (require exact => starts from 0 and has same length)
    if (pos != string::npos &&
        (!_exact || (pos == 0 && candidate.length() == name.length()))) {
      return _accept;
    }
  }
  return !_accept;
}

/** Checks MethodInfo against both filters.
 *
 * This can be 2 Black-/2 WhitelistFilter (not that useful) or one of each type.
 *
 * @param info
 * @return
 */
bool DualFilter::accept(const art::MethodInfo& info) const {
  return this->filter1->accept(info) && this->filter2->accept(info);
}

}  // namespace art
