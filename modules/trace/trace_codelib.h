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

#ifndef ART_MODULES_TRACE_TRACE_CODELIB_H_
#define ART_MODULES_TRACE_TRACE_CODELIB_H_

#include <string>
#include <unordered_set>
#include "optimizing/artist/env/codelib.h"

using std::string;
using std::unordered_set;

namespace art {

class TraceCodeLib : public CodeLib {
 public:
  static const string TRACELOG;

 public:
  TraceCodeLib() {}
  virtual ~TraceCodeLib() {}
  virtual unordered_set<string>& getMethods() const;
  virtual string& getInstanceField() const;
  virtual string& getCodeClass() const;
};  // class TraceCodeLib

}  // namespace art

#endif  // ART_MODULES_TRACE_TRACE_CODELIB_H_
