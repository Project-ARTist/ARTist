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

#ifndef ART_API_MODULES_METHOD_INFO_H_
#define ART_API_MODULES_METHOD_INFO_H_

#include <string>
#include <vector>
#include "optimizing/nodes.h"
#include "driver/dex_compilation_unit.h"
#include "optimizing/artist/api/utils/artist_utils.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace art {

/**
 * Wraps information about and convenience methods to work with the current method.
 */
class ArtistMethodInfo {
 public:
  ArtistMethodInfo(HGraph* methodGraph, const DexCompilationUnit& compUnit);

  // method
  const string& GetMethodName(bool signature = false) const;
  bool IsStatic() const;
  HGraph* GetGraph() const;

  // parameters
  const vector<HParameterValue*>& GetParams() const;
  const vector<string>& GetParamTypes() const;
  bool IsStringParam(HParameterValue* param) const;
  bool IsThisParameter(HParameterValue* param) const;

 private:
  HGraph* graph;
  const DexCompilationUnit& compilationUnit;
  const string methodName;
  const string methodNameWithSignature;
  vector<HParameterValue*> params;
  vector<string> paramTypes;

  friend class MethodInfoFactory;
};

ostream& operator<<(ostream& os, const ArtistMethodInfo& info);

}  // namespace art

#endif  // ART_API_MODULES_METHOD_INFO_H_
