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

#ifndef ART_ARTIST_METHOD_VISITOR_H_
#define ART_ARTIST_METHOD_VISITOR_H_

#include "optimizing/nodes.h"

namespace art {

class HArtist;
class HGraph;
class HInvoke;
class HSink;
class HParameterValue;
class HInstruction;
class HInvokeVirtual;
class HInvokeStaticOrDirect;
class HInstanceFieldSet;
class HInvokeInterface;
class HReturn;
class HReturnVoid;
class HGraphVisitor;
class HArtistMethodVisitor;
class HStaticFieldSet;


class HArtistMethodVisitor : public HGraphVisitor {
 public:
  explicit HArtistMethodVisitor(HGraph* graph);

  virtual void VisitInstruction(HInstruction* instruction) OVERRIDE;

  virtual void VisitInvokeInterface(HInvokeInterface* instruction) OVERRIDE;
  virtual void VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) OVERRIDE;
  virtual void VisitInvokeVirtual(HInvokeVirtual* instruction) OVERRIDE;
  virtual void VisitStaticFieldSet(HStaticFieldSet* instruction) OVERRIDE;
  virtual void VisitInstanceFieldSet(HInstanceFieldSet* instruction) OVERRIDE;
  virtual void VisitParameterValue(HParameterValue* instruction) OVERRIDE;
  virtual void VisitReturn(HReturn* instruction) OVERRIDE;
  virtual void VisitReturnVoid(HReturnVoid* instruction) OVERRIDE;
};

}  // namespace art

#endif  // ART_ARTIST_METHOD_VISITOR_H_
