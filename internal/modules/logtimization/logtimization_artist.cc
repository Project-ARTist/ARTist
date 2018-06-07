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

#include "runtime.h"

#include "logtimization_artist.h"
#include "optimizing/artist/api/env/codelib_environment.h"
#include "optimizing/artist/api/io/artist_log.h"


using std::string;
using std::vector;
using std::shared_ptr;

namespace art {

HLogtimization::HLogtimization(const MethodInfo& method_info,
#ifdef BUILD_MARSHMALLOW
                               bool is_in_ssa_form,
#endif
                               const char* pass_name,
                               OptimizingCompilerStats* stats)
      : HArtist(method_info
#ifdef BUILD_MARSHMALLOW
      , is_in_ssa_form
#endif
    , pass_name, stats) {}

HLogtimization::~HLogtimization() = default;

void HLogtimization::RunPass() {
  CHECK(graph_ != nullptr);
  VLOG(artist) << "<##########################################################>";

  const DexFile& dexFile = graph_->GetDexFile();
  const string& dexFilePath = dexFile.GetLocation();
  const string methodSignature = PrettyMethod(graph_->GetMethodIdx(), dexFile);

  VLOG(artist) << "Method: " << methodSignature << " (Dexfile: " << dexFilePath << ")";
  VLOG(artist) << dexFile;
  const auto class_count = dexFile.NumClassDefs();
  const auto field_count = dexFile.NumFieldIds();
  const auto method_count = dexFile.NumMethodIds();
  const auto proto_count = dexFile.NumProtoIds();
  const auto string_count = dexFile.NumStringIds();
  const auto type_count = dexFile.NumTypeIds();

  VLOG(artist) << "C: " << class_count << " "
                << "F: " << field_count << " "
                << "M: " << method_count << " "
                << "P: " << proto_count << " "
                << "S: " << string_count << " "
                << "T: " << type_count;

  VLOG(artist) << "</##########################################################>";
}

}  // namespace art
