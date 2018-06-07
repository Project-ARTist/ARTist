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

#ifndef ART_INTERNAL_INJECTION_INJECTION_VISITOR_H_
#define ART_INTERNAL_INJECTION_INJECTION_VISITOR_H_

#include "optimizing/artist/api/injection/injection_artist.h"
#include "optimizing/artist/api/injection/injection.h"
#include "optimizing/artist/api/env/codelib_environment.h"

namespace art {

class HInjectionVisitor : public HGraphVisitor {
 private:
  HInjectionArtist* artist;
  HGraph* graph;

 private:
  void InjectInstruction(HInstruction* instruction, shared_ptr<const Injection>& injection);

  string GetInvokedMethod(HInstruction* instruction);

 public:
  explicit HInjectionVisitor(HInjectionArtist* parent_artist, HGraph* method_graph);

 public:
  static bool MethodSignatureStartsWith(const string& method_signature, const string& search_string);

  static bool MethodSignatureContains(const string& method_signature, const string& search_string);

 public:
  virtual void VisitInvoke(HInvoke* instruction) OVERRIDE;
  virtual void VisitInvokeInterface(HInvokeInterface* instruction) OVERRIDE;
  virtual void VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) OVERRIDE;
  virtual void VisitInvokeVirtual(HInvokeVirtual* instruction) OVERRIDE;
  virtual void VisitReturn(HReturn* instruction) OVERRIDE;
  virtual void VisitReturnVoid(HReturnVoid* instruction) OVERRIDE;
};

}  // namespace art

#endif  // ART_INTERNAL_INJECTION_INJECTION_VISITOR_H_
