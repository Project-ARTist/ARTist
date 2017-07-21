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

#include "artist.h"
#include "artist_log.h"
#include "blacklist.h"
#include "injection/injection_visitor.h"
#include "env/java_env.h"
#include "class_linker.h"
#include "class_linker-inl.h"
#include "method_info_factory.h"
#include "driver/compiler_driver.h"

#include "optimizing/builder.h"

#include "injection/primitives.h"
#include "injection/boolean.h"
#include "injection/integer.h"
#include "injection/float.h"


using std::string;
using std::vector;
using std::shared_ptr;
using std::endl;
using std::sort;

namespace art {

uint32_t HArtist::method_counter = 0;

HArtist::HArtist(HGraph* graph,
                 const DexCompilationUnit& _dex_compilation_unit,
#ifdef BUILD_MARSHMALLOW
                 bool is_in_ssa_form,
#endif
                 const char* pass_name,
                 OptimizingCompilerStats* stats)
    : HOptimization(graph,
#ifdef BUILD_MARSHMALLOW
                    is_in_ssa_form,
#endif
                    pass_name, stats)
    , code_lib(nullptr)
    , env(nullptr)
    , methodInfo(MethodInfoFactory::Obtain(graph, _dex_compilation_unit)) {
  ArtistLog::SetupArtistLogging();
  const string& VERSION = "00109";
  LogVersionOnce(VERSION);
}

// TODO move out of multithreaded area
void HArtist::LogVersionOnce(const string& VERSION) const {
  static std::atomic_flag version_logged = ATOMIC_FLAG_INIT;

  if (!version_logged.test_and_set()) {
    VLOG(artist) << std::endl
                 << "HArtist() Version: " << VERSION << std::endl;
  }
}

HArtist::~HArtist() {
  VLOG(artistd) << "~HArtist()" << std::endl;
  delete methodInfo;
}

void HArtist::Run() {
  CHECK(graph_ != nullptr);

  const std::string methodName = methodInfo->GetMethodName();
  const std::string& dexFileName = graph_->GetDexFile().GetLocation();

  if (BlackList::IsBlacklisted(methodName)) {
    VLOG(artistd) << "HArtist::Run() SKIPPING " << methodName << " (" << dexFileName << ")";;
    return;
  }
  if (env->IsCodeLib(graph_->GetDexFile())) {
    VLOG(artistd) << "HArtist::Run() SKIPPING CodeLib " << methodName << " (" << dexFileName << ")";
    return;
  }
  const string method_signature = PrettyMethod(graph_->GetMethodIdx(), graph_->GetDexFile());
  ArtistLog::LogMethodCount(++method_counter);
  VLOG(artistd) << std::endl;
  VLOG(artistd) << "Artist #" << method_counter << ": " << method_signature<< " (" << dexFileName << ")";

  Setup();
  RunModule();
}

void HArtist::Setup() {
  VLOG(artist) << "HArtist::Setup()" << std::endl;

  SetupModule();

  VLOG(artist) << "HArtist::Setup() Done" << std::endl;
}


HInstruction* HArtist::GetCodeLib(HInstruction* instruction_cursor) {
  if (this->code_lib == nullptr) {
    if (instruction_cursor == nullptr) {
      this->code_lib = ArtUtils::InjectCodeLib(graph_->GetEntryBlock()->GetLastInstruction(), env);
    } else {
      this->code_lib = ArtUtils::InjectCodeLib(instruction_cursor, env, false);
    }
  }
  return this->code_lib;
}

const MethodInfo* HArtist::GetMethodInfo() const {
  VLOG(artistd) << "HArtist::GetMethodInfo(): " << this->methodInfo << std::flush;
  return this->methodInfo;
}

/**
 * Stub implementations that need to be overwritten by concrete modules.
 */
void HArtist::SetupModule() {
  VLOG(artist) << "HArtist::SetupModule(): No-op, override this method in the concrete module. ";
}

void HArtist::RunModule() {
  VLOG(artist) << "HArtist::RunModule(): No-op, override this method in the concrete module. ";
}

void HArtist::setCodeLibEnvironment(CodeLibEnvironment *environment) {
  this->env = environment;
}

CodeLibEnvironment *HArtist::getCodeLibEnvironment() {
  return env;
}

}  // namespace art
