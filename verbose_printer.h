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

#ifndef ART_VERBOSE_PRINTER_H_
#define ART_VERBOSE_PRINTER_H_

#include "optimizing/pretty_printer.h"
#include "method_info.h"

namespace art {

  class VerbosePrinter : public StringPrettyPrinter {
  public:
    explicit VerbosePrinter(const ArtistMethodInfo& info);
    explicit VerbosePrinter(HGraph* graph, const DexCompilationUnit& dex_compilation_unit);

    ~VerbosePrinter() {}

    void VisitNewInstance(HNewInstance* instruction) OVERRIDE;
    void VisitLoadString(HLoadString* instruction) OVERRIDE;
    void VisitParameterValue(HParameterValue* instruction) OVERRIDE;
    void VisitLoadClass(HLoadClass* instruction) OVERRIDE;
    void VisitInvokeVirtual(HInvokeVirtual* instruction) OVERRIDE;
    void VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) OVERRIDE;
    void VisitInvokeInterface(HInvokeInterface* instruction) OVERRIDE;
    void VisitIntConstant(HIntConstant* instruction) OVERRIDE;
    void VisitLongConstant(HLongConstant* instruction) OVERRIDE;
    void PrintPostInstruction(HInstruction* instruction);

  protected:  // methods
    void PrintInvoke(HInvoke* invoke);
    void PrintLong(int64_t value);
  protected:  // fields
    const ArtistMethodInfo methodInfo;
  };

}  // namespace art

#endif  // ART_VERBOSE_PRINTER_H_
