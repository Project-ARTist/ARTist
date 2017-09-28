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

#include <sstream>
#include <inttypes.h>
#include "verbose_printer.h"
#include "artist_utils.h"
#include "dex_file.h"
#include "dex_file-inl.h"
#include "method_info_factory.h"

#ifdef BUILD_OREO
#include "android-base/stringprintf.h"
using android::base::StringPrintf;
#endif

namespace art {

  VerbosePrinter::VerbosePrinter(const ArtistMethodInfo& info)
      : StringPrettyPrinter(info.GetGraph()), methodInfo(info) { }

  VerbosePrinter::VerbosePrinter(HGraph* graph, const DexCompilationUnit& dex_compilation_unit)
      : StringPrettyPrinter(graph), methodInfo(MethodInfoFactory::obtain(graph, dex_compilation_unit)) {}

  void VerbosePrinter::VisitNewInstance(art::HNewInstance *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString(": ");
    std::stringstream strstream;
    strstream << GetGraph()->GetDexFile().StringByTypeIdx(instruction->GetTypeIndex());
    PrintString(strstream.str().c_str());
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::VisitLoadString(HLoadString *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString(": '");
    PrintString(GetGraph()->GetDexFile().StringDataByIdx(instruction->GetStringIndex()));
    PrintString("'");
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::VisitParameterValue(HParameterValue *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString("(");
    Primitive::Type type = instruction->GetType();
    if (methodInfo.IsThisParameter(instruction)) {
      PrintString("this");
    } else if (type != Primitive::Type::kPrimNot) {
      std::stringstream strstream;
      strstream << type;
      PrintString(strstream.str().c_str());
    } else {
      // TODO obtain actual type
      VLOG(compiler) << "VerbosePrinter:Param:complex type TODO";
      PrintString("Object type");
    }
    PrintString(")");
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::VisitLoadClass(HLoadClass *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString(": ");
    PrintString(GetGraph()->GetDexFile().GetTypeDescriptor(GetGraph()->GetDexFile().GetTypeId((instruction->GetTypeIndex()))));
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::VisitInvokeVirtual(HInvokeVirtual *instruction) {
    PrintInvoke(instruction);
  }

  void VerbosePrinter::VisitInvokeStaticOrDirect(HInvokeStaticOrDirect *instruction) {
    PrintInvoke(instruction);
  }

  void VerbosePrinter::VisitInvokeInterface(HInvokeInterface *instruction) {
    PrintInvoke(instruction);
  }

  void VerbosePrinter::VisitIntConstant(HIntConstant *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString(": ");
    PrintInt(instruction->GetValue());
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::VisitLongConstant(HLongConstant *instruction) {
    PrintPreInstruction(instruction);
    PrintString(instruction->DebugName());
    PrintString(": ");
    PrintLong(instruction->GetValue());
    PrintPostInstruction(instruction);
  }

  void VerbosePrinter::PrintInvoke(HInvoke *invoke) {
    PrintPreInstruction(invoke);
    std::string method_name = ArtUtils::GetMethodName(invoke);
    PrintString(invoke->DebugName());
    PrintString(": ");
    PrintString(method_name.c_str());
    PrintPostInstruction(invoke);
  }

  void VerbosePrinter::PrintLong(int64_t value) {
    PrintString(StringPrintf("%" PRId64, value).c_str());
  }

  void VerbosePrinter::PrintPostInstruction(HInstruction* instruction) {
    if (instruction->InputCount() != 0) {
      HInputsRef inputs = instruction->GetInputs();
      PrintString("(");
      bool first = true;
      for (auto && input : inputs) {
        if (first) {
          first = false;
        } else {
          PrintString(", ");
        }
        PrintInt(input->GetId());
      }
      PrintString(")");
    }
    if (instruction->HasUses()) {
      if (instruction->InputCount() != 0) {
        PrintString(",");
      }
      PrintString(" uses: [");

      bool first = true;
#ifdef BUILD_MARSHMALLOW
      for (HUseIterator<HInstruction*> it(instruction->GetUses()); !it.Done(); it.Advance()) {
        if (first) {
          first = false;
        } else {
          PrintString(", ");
        }
        PrintInt(it.Current()->GetUser()->GetId());
      }
#else
      for (auto && it : instruction->GetUses()) {
        if (first) {
          first = false;
        } else {
          PrintString(", ");
        }
        PrintInt(it.GetUser()->GetId());
      }
#endif
      PrintString("]");
    }
    PrintNewLine();
  }
}  // namespace art
