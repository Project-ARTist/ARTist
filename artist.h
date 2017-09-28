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

#ifndef ART_ARTIST_H_
#define ART_ARTIST_H_

#include "env/codelib_environment.h"
#include "injection/injection.h"
#include "method_info.h"
#include "optimizing/optimization.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace art {

class HGraph;
class HInstruction;
class DexCompilationUnit;
class CompilerDriver;
class OptimizingCompilerStats;


/**
 * HArtist is ARTist's interface for custom "optimization passes". We exploit the Optimizing backend's optimization
 * infrastructure to execute our own passes as if they were optimizations.
 * An ARTist pass has full access to the current method's code and can modify it at will.
 */
class HArtist : public HOptimization {
 public:
  HArtist(HGraph *graph,
          const DexCompilationUnit& _dex_compilation_unit,
#ifdef BUILD_MARSHMALLOW
          bool is_in_ssa_form = true,
#endif
          const char *pass_name = "ArtistInstrumentation",
          OptimizingCompilerStats *stats = nullptr);

  HInstruction* GetCodeLibInstruction(HInstruction *instruction_cursor = nullptr);

  const ArtistMethodInfo& GetMethodInfo() const;

  void setDexfileEnvironment(shared_ptr<const DexfileEnvironment> environment);
  shared_ptr<const DexfileEnvironment> getDexfileEnvironment() const;

  void setCodeLibEnvironment(shared_ptr<CodeLibEnvironment> environment);
  shared_ptr<CodeLibEnvironment> getCodeLibEnvironment() const;

  void Run() OVERRIDE;

 private:
  HInstruction* codelib_instruction;
  shared_ptr<const DexfileEnvironment> _dexfile_env;
  shared_ptr<CodeLibEnvironment> _codelib_env;

 protected:
  const ArtistMethodInfo method_info;

  static uint32_t method_counter;

 protected:
  void LogVersionOnce(const string& VERSION) const;
  void Setup();

  // Module API

  /**
   * Initializes the module. Concrete modules can but do not have to overwrite this function. The default implementation
   * is a no-op.
   */
  virtual void SetupModule();

  /**
   * Execute the module's actual logic. All operations that change the current method should be done here.
   */
  virtual void RunModule() = 0;
};

}  // namespace art

#endif  // ART_ARTIST_H_
