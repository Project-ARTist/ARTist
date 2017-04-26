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

#include <atomic>

#include "trace_artist.h"
#include "optimizing/artist/artist_log.h"
#include "optimizing/artist/injection/injection_visitor.h"
#include "optimizing/artist/env/java_env.h"
#include "optimizing/artist/env/codelib.h"
#include "class_linker.h"
#include "class_linker-inl.h"

#include "optimizing/artist/injection/primitives.h"
#include "optimizing/artist/injection/boolean.h"
#include "optimizing/artist/injection/integer.h"
#include "optimizing/artist/injection/float.h"

#include "optimizing/artist/verbose_printer.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::endl;
using std::sort;


namespace art {


/* setup */


void HTraceArtist::SetupModule() {
  // Locked by Environment
  const std::string& dex_name = ArtUtils::GetDexFileName(graph_);
  SetupEnvironment(dex_name);

  static std::atomic_flag setup_ready = ATOMIC_FLAG_INIT;
  if (!setup_ready.test_and_set()) {
    std::vector<Injection> injections;
    const std::string METHOD_SIGNATURE_TRACELOG =
        CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__TRACELOG____V;

    // TODO Bug: Using Injection with multiple targets that have the same InjectionTarget
    //           Leads to duplicate injections
    std::vector<shared_ptr<Parameter>> empty_Params;

    Target target_one(Target::GENERIC_TARGET, InjectionTarget::METHOD_END);

    Injection injection(METHOD_SIGNATURE_TRACELOG, empty_Params, target_one);
    injections.push_back(injection);

    SetupInjections(injections);
  }
}

void HTraceArtist::SetupInjections(std::vector<Injection>& injection_list) {
  static std::atomic_flag injections_ready = ATOMIC_FLAG_INIT;

  if (!injections_ready.test_and_set()) {
    VLOG(artistd) << "SetupInjections()";
    CodeLibEnvironment& codeLib = CodeLibEnvironment::GetInstance();
    codeLib.SetupInjections(injection_list);
    VLOG(artistd) << "SetupInjections() DONE";
    VLOG(artistd) << std::endl;
    VLOG(artistd) << std::endl;
  }
}

void HTraceArtist::SetupEnvironment(const std::string& dex_name) {
  VLOG(artistd) << "SetupEnvironment()";
  CodeLibEnvironment& codeLib = CodeLibEnvironment::GetInstance();
  const std::vector<const DexFile*> dex_files;
  codeLib.SetupEnvironment(dex_files, dex_name, graph_->GetDexFile(), nullptr, nullptr);
  VLOG(artistd) << "SetupEnvironment() DONE";
  VLOG(artistd) << std::endl;
  VLOG(artistd) << std::endl;
}

void HTraceArtist::RunModule()  {
  VLOG(artistd) << "RunTaskInjection()";

  // printGraph("### Before Artist");

  CodeLibEnvironment& env = CodeLibEnvironment::GetInstance();

  HInjectionVisitor injectionVisitor(this, graph_, env.GetInjections());
  injectionVisitor.VisitInsertionOrder();

  printGraph("### After Artist");

  VLOG(artistd) << "RunTaskInjection() DONE";
}

void HTraceArtist::printGraph(const std::string& message) const {
  VLOG(artistd) << message;
  VerbosePrinter verbosePrinter(graph_, dex_compilation_unit);
  verbosePrinter.VisitReversePostOrder();
  VLOG(artistd) << verbosePrinter.str();
  VLOG(artistd) << message << " DONE";
}

}  // namespace art
