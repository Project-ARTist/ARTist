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
#include "injection/injection_visitor.h"
#include "env/java_env.h"
#include "method_info_factory.h"
#include "driver/compiler_driver.h"
#include "optimizing/artist/env/dexfile_environment.h"

#include "optimizing/builder.h"

#include "injection/primitives.h"
#include "injection/boolean.h"
#include "injection/integer.h"
#include "injection/float.h"


using std::vector;
using std::shared_ptr;
using std::endl;
using std::sort;
using std::atomic_flag;

namespace art {

uint32_t HArtist::_method_counter = 0;

HArtist::HArtist(const MethodInfo& method_info,
#ifdef BUILD_MARSHMALLOW
                 bool is_in_ssa_form,
#endif
                 const char* pass_name,
                 OptimizingCompilerStats* stats)
    : HOptimization(method_info.GetGraph(),
#ifdef BUILD_MARSHMALLOW
                    is_in_ssa_form,
#endif
                    pass_name, stats)
    , _codelib_instruction(nullptr)
    , _method_info(method_info) {
}

void HArtist::Run() {
  CHECK(graph_ != nullptr);
  VLOG(artistd) << "HArtist::Run()";

  const string method_name = _method_info.GetMethodName();
  const string& dexFileName = graph_->GetDexFile().GetLocation();

  if (_dexfile_env->isCodelib(&graph_->GetDexFile())) {
    VLOG(artistd) << "HArtist::Run() SKIPPING CodeLib " << method_name << " (" << dexFileName << ")";
    return;
  }
  const string method_signature = PrettyMethod(graph_->GetMethodIdx(), graph_->GetDexFile());
  ArtistLog::LogMethodCount(++_method_counter);
  VLOG(artistd) << std::endl;
  VLOG(artistd) << "Artist #" << _method_counter << ": " << method_signature<< " (" << dexFileName << ")";

  Setup();
  RunPass();
}

void HArtist::Setup() {
  VLOG(artistd) << "HArtist::Setup()" << std::endl;

  SetupPass();

  VLOG(artistd) << "HArtist::Setup() Done" << std::endl;
}


HInstruction* HArtist::GetCodeLibInstruction(HInstruction *instruction_cursor) {
  if (this->_codelib_instruction == nullptr) {
    if (instruction_cursor == nullptr) {
      this->_codelib_instruction = ArtUtils::InjectCodeLib(graph_->GetEntryBlock()->GetLastInstruction(), _codelib_env);
    } else {
      this->_codelib_instruction = ArtUtils::InjectCodeLib(instruction_cursor, _codelib_env, false);
    }
  }
  return this->_codelib_instruction;
}

const MethodInfo& HArtist::GetMethodInfo() const {
  VLOG(artistd) << "HArtist::GetMethodInfo(): " << this->_method_info << std::flush;
  return this->_method_info;
}

/**
 * Stub implementation that can but does not have to be overwritten in the concrete module.
 */
void HArtist::SetupPass() {
  VLOG(artistd) << "HArtist::SetupPass(): No-op.";
}

void HArtist::setDexfileEnvironment(shared_ptr<const DexfileEnvironment> environment) {
    _dexfile_env = environment;
}

shared_ptr<const DexfileEnvironment> HArtist::getDexfileEnvironment() const {
    return _dexfile_env;
}

void HArtist::setCodeLibEnvironment(shared_ptr<CodeLibEnvironment> environment) {
  this->_codelib_env = environment;
}

shared_ptr<CodeLibEnvironment> HArtist::getCodeLibEnvironment() const {
  return _codelib_env;
}

}  // namespace art
