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

#include "trace_module.h"
#include "trace_artist.h"
#include "trace_codelib.h"

namespace art {

    HArtist* TraceModule::createPass(HGraph *graph, const DexCompilationUnit &dex_compilation_unit) const {
        return new /*(graph->GetArena())*/ HTraceArtist(graph, dex_compilation_unit);
    }

    const CodeLib* TraceModule::createCodeLib() const {
        return new TraceCodeLib();
    }
}  // namespace art
