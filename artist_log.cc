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

#include "artist_method_visitor.h"
#include "artist.h"
#include "artist_log.h"
#include "verbose_printer.h"
#include "artist_utils.h"
#include "optimizing/nodes.h"

#include "optimizing/artist/verbose_printer.h"

namespace art {

  const bool ArtistLog::VERBOSE_LOGGING = false;

  const std::string ArtistLog::VERSION = "00001";  // Debug and verification help

  void ArtistLog::LogMethod(const ArtistMethodInfo& info) {
    VLOG(artistd) << "Method " << info.GetMethodName(true);
    VerbosePrinter printer(info);
    printer.VisitReversePostOrder();
    VLOG(artistd) << printer.str();
  }

  void ArtistLog::LogMethod(const ArtistMethodInfo& info, std::ostream &out) {
    out << "Method " << info.GetMethodName(true);
    VerbosePrinter printer(info);
    printer.VisitReversePostOrder();
    out << printer.str();
  }

  void ArtistLog::LogInstruction(HInstruction *instruction, const DexCompilationUnit& dex_compilation_unit) {
    VerbosePrinter printer(instruction->GetBlock()->GetGraph(), dex_compilation_unit);
    instruction->Accept(&printer);
    VLOG(compiler) << printer.str();
  }

  void ArtistLog::LogInstruction(HInstruction *instruction, const DexCompilationUnit& dex_compilation_unit, std::ostream &out) {
    VerbosePrinter printer(instruction->GetBlock()->GetGraph(), dex_compilation_unit);
    instruction->Accept(&printer);
    out << printer.str();
  }

  void ArtistLog::LogInstructionName(const HInstruction *instruction) {
    if (ArtistLog::VERBOSE_LOGGING) {
      VLOG(compiler) << "InstructionName: " << instruction->DebugName();
    }
  }

  void ArtistLog::LogMethodCount(const uint32_t method_count) {
    if ((method_count % 1000) == 0) {
      VLOG(artist) << "Artist Handling Method: ##" << method_count;
    }
  }

  void ArtistLog::SetupArtistLogging() {
    static std::atomic_flag logging_enabled = ATOMIC_FLAG_INIT;
    // One Time setup, that vars are available
    if (!logging_enabled.test_and_set()) {
      gLogVerbosity.artist = true;
      gLogVerbosity.artistd = false;
//        gLogVerbosity.class_linker = true;
//        gLogVerbosity.compiler = true;
//        gLogVerbosity.gc = true;
//        gLogVerbosity.heap = true;
//        gLogVerbosity.jdwp = true;
//        gLogVerbosity.jit = true;
//        gLogVerbosity.jni = true;
//        gLogVerbosity.monitor = true;
//        gLogVerbosity.oat = true;
//        gLogVerbosity.profiler = true;
//        gLogVerbosity.signals = true;
//        gLogVerbosity.startup = true;
//        gLogVerbosity.third_party_jni = true;
//        gLogVerbosity.threads = true;
//        gLogVerbosity.verifier = true;
    }
    VLOG(artist) << "% ARTist Version " << ArtistLog::VERSION;
  }

  void ArtistLog::ForceVerboseLogging() {
    gLogVerbosity.artist = true;
    gLogVerbosity.artistd = true;
    gLogVerbosity.class_linker = true;  // Enabled with "-verbose:class".
    gLogVerbosity.compiler = true;
    gLogVerbosity.gc = true;
    gLogVerbosity.heap = true;
    gLogVerbosity.jdwp = true;
    gLogVerbosity.jit = true;
    gLogVerbosity.jni = true;
    gLogVerbosity.monitor = true;
    gLogVerbosity.oat = true;
    gLogVerbosity.profiler = true;
    gLogVerbosity.signals = true;
    gLogVerbosity.startup = true;
    gLogVerbosity.third_party_jni = true;  // Enabled with "-verbose:third-party-jni".
    gLogVerbosity.threads = true;
    gLogVerbosity.verifier = true;
#ifndef BUILD_MARSHMALLOW
    gLogVerbosity.collector = true;
    gLogVerbosity.deopt = true;
    gLogVerbosity.simulator = true;
    gLogVerbosity.image = true;
    gLogVerbosity.systrace_lock_logging = true;  // Enabled with "-verbose:sys-locks".
#endif
  }

}  // namespace art
