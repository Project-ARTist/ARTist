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

#include "logtimization_module.h"
#include "logtimization_artist.h"
#include "optimizing/artist/filtering/method_name_filters.h"

using std::make_shared;

namespace art {

shared_ptr<HArtist> LogtimizationModule::createPass(const MethodInfo& method_info) const {
//  return new /*(graph->GetArena())*/ HLogtimization(method_info);
  return make_shared<HLogtimization>(method_info);
}

shared_ptr<const CodeLib> LogtimizationModule::createCodeLib() const {
  return nullptr;
}

/* (Arbitrary) restriction of compile-time output to "onCreate" methods (Activities, Applications, ...) to reduce
 * logcat spamming.
 */
std::pair<unique_ptr<Filter>, unique_ptr<Filter>> LogtimizationModule::getMethodFilters() const {
  const vector<const string> whiteListSignatures = {
      ".onCreate("
  };
  auto whiteListFilter = unique_ptr<Filter>(new WhitelistFilter(whiteListSignatures, false, true));
//  const vector<const string> blackListSignatures = {};
//  auto blackListFilter = new BlacklistFilter(blackListSignatures, false, true);
  return std::make_pair(unique_ptr<Filter>(nullptr), std::move(whiteListFilter));
}

}  // namespace art
