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

#include "filter.h"
#include <vector>
#include <string>

#ifndef ART_FILTERING_METHOD_NAME_FILTERS_H_
#define ART_FILTERING_METHOD_NAME_FILTERS_H_

using std::vector;
using std::string;
using std::unique_ptr;

namespace art {

/**
 * A filter implementation based on a list of method names.
 */
class MethodNameFilter : public Filter {
 public:
  MethodNameFilter(const vector<const string>& method_names, bool accept_or_deny, bool exact = false,
                   bool signature = false)
      : _names(method_names), _accept(accept_or_deny), _exact(exact), _signature(signature) {}

  MethodNameFilter(const MethodNameFilter& other) = default;
  MethodNameFilter(MethodNameFilter&& other) = default;

  ~MethodNameFilter() = default;

  MethodNameFilter& operator= (const MethodNameFilter& other) = default;
  MethodNameFilter& operator= (MethodNameFilter&& other) = default;

  bool accept(const MethodInfo &info) const OVERRIDE;

 private:
  const vector<const string> _names;
  const bool _accept;
  const bool _exact;
  const bool _signature;
};

/**
 * A filter implementation that checks candidate method names against a whitelist.
 */
class WhitelistFilter : public MethodNameFilter {
 public:
  WhitelistFilter(const vector<const string>& method_names, bool exact = false, bool signature = false) :
      MethodNameFilter(method_names, true, exact, signature) {}
};

/**
 * A filter implementation that checks candidate method names against a blacklist.
 */

class BlacklistFilter : public MethodNameFilter {
 public:
  BlacklistFilter(const vector<const string>& method_names, bool exact = false, bool signature = false) :
      MethodNameFilter(method_names, false, exact, signature) {}
};

/**
 * A filter implementation that checks method names against two filter,
 * which enables one to create a WhiteList with Blacklisted exceptions.
 */
class DualFilter : public Filter {
  const unique_ptr<Filter> filter1;
  const unique_ptr<Filter> filter2;

 public:
  DualFilter(unique_ptr<Filter>& _filter1,
             unique_ptr<Filter>& _filter2)
  : filter1(std::move(_filter1))
  , filter2(std::move(_filter2)) { }

  bool accept(const MethodInfo& info) const OVERRIDE;
};

}  // namespace art

#endif  // ART_FILTERING_METHOD_NAME_FILTERS_H_
