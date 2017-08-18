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

#ifndef ART_ARTIST_LOG_H_
#define ART_ARTIST_LOG_H_

#include "method_info.h"
#include "base/logging.h"

namespace art {

  class HGraph;
  class HInstruction;

  class ArtistLog {
  private:
  public:
    static const std::string PREFIX;
    static const bool VERBOSE_LOGGING;

    static void SetupArtistLogging();

    static void ForceVerboseLogging();

    static void LogMethod(const MethodInfo& info);
    static void LogMethod(const MethodInfo& info, std::ostream& out);

    static void LogInstruction(HInstruction* instruction,
                               const DexCompilationUnit& _dex_compilation_unit);
    static void LogInstruction(HInstruction* instruction,
                               const DexCompilationUnit& _dex_compilation_unit,
                               std::ostream& out);

    static void LogInstructionName(const HInstruction* instruction);

    static void LogMethodCount(const uint32_t method_count);
  };
}  // namespace art

#endif  // ART_ARTIST_LOG_H_
