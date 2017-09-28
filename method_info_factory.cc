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

#include "method_info_factory.h"
#include "param_finder.h"
#include "dex_file-inl.h"

namespace art {

const ArtistMethodInfo MethodInfoFactory::obtain(HGraph *method_graph, const DexCompilationUnit &comp_unit) {
  ArtistMethodInfo info(method_graph, comp_unit);

  string signature_string = method_graph->GetDexFile().GetMethodSignature(
      method_graph->GetDexFile().GetMethodId(method_graph->GetMethodIdx())).ToString();

  ParamFinder visitor(method_graph);
  visitor.VisitReversePostOrder();

  // searching method parameters
  info.params = visitor.GetFoundParameters();
  ArtUtils::ExtractMethodArguments(signature_string, info.paramTypes);

  return info;
}

};  // namespace art
