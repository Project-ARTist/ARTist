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

#ifndef ART_MODULES_METHOD_TRACING_TRACE_ARTIST_H_
#define ART_MODULES_METHOD_TRACING_TRACE_ARTIST_H_

#include "optimizing/artist/injection/universal_artist.h"

namespace art {

class HTraceArtist : public HUniversalArtist {
 public:
  HTraceArtist(
      HGraph* graph,
      const DexCompilationUnit& _dex_compilation_unit,
#ifdef BUILD_MARSHMALLOW
      bool is_in_ssa_form = true,
#endif
      const char* pass_name = "TraceArtist"
      , OptimizingCompilerStats* stats = nullptr)
      : HUniversalArtist(graph
      , _dex_compilation_unit
#ifdef BUILD_MARSHMALLOW
      , is_in_ssa_form
#endif
      , pass_name
      , stats) {
    // Nothing
  }

  void SetupModule() OVERRIDE;
};

}  // namespace art

#endif  // ART_MODULES_METHOD_TRACING_TRACE_ARTIST_H_
