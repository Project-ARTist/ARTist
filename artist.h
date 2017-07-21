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
   * @author "Oliver Schranz <oliver.schranz@cispa.saarland>"
   * @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
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

    HInstruction* GetCodeLib(HInstruction* instruction_cursor = nullptr);

    const MethodInfo* GetMethodInfo() const;

    void setCodeLibEnvironment(CodeLibEnvironment* environment);
    CodeLibEnvironment* getCodeLibEnvironment();

    virtual ~HArtist();

    void Run() OVERRIDE;

   private:  // Member Vars
    HInstruction* code_lib;
    CodeLibEnvironment* env;

  protected:  // Member Vars
    const MethodInfo* methodInfo;

    static uint32_t method_counter;

  protected:  // Member Functions
    void LogVersionOnce(const string& VERSION) const;
    void Setup();


    // to be overwritten by modules
    virtual void SetupModule();
    virtual void RunModule();
  };

}  // namespace art

#endif  // ART_ARTIST_H_
