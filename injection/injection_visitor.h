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

#ifndef ART_INJECTION_INJECTION_VISITOR_H_
#define ART_INJECTION_INJECTION_VISITOR_H_

#include "universal_artist.h"
#include "injection.h"
#include "optimizing/artist/artist_method_visitor.h"
#include "optimizing/artist/env/codelib_environment.h"

namespace art {

class HInjectionVisitor : public HArtistMethodVisitor {
 private:
  shared_ptr<HUniversalArtist> artist;
  HGraph* graph;

 private:
  void InjectInstruction(HInstruction* instruction, const Injection& injection);

  string GetInvokedMethod(HInstruction* instruction);

 public:
  explicit HInjectionVisitor(shared_ptr<HUniversalArtist> parent_artist, HGraph* method_graph);

 public:
  static bool MethodSignatureStartsWith(const string& method_signature, const string& search_string);

  static bool MethodSignatureContains(const string& method_signature, const string& search_string);

 public:
  virtual void VisitAdd(HAdd* instruction) OVERRIDE;
  virtual void VisitAnd(HAnd* instruction) OVERRIDE;
  virtual void VisitArrayGet(HArrayGet* instruction) OVERRIDE;
  virtual void VisitArrayLength(HArrayLength* instruction) OVERRIDE;
  virtual void VisitArraySet(HArraySet* instruction) OVERRIDE;
  virtual void VisitBinaryOperation(HBinaryOperation* instruction) OVERRIDE;
  virtual void VisitBooleanNot(HBooleanNot* instruction) OVERRIDE;
  virtual void VisitBoundsCheck(HBoundsCheck* instruction) OVERRIDE;
  virtual void VisitBoundType(HBoundType* instruction) OVERRIDE;
  virtual void VisitCheckCast(HCheckCast* instruction) OVERRIDE;
  virtual void VisitClinitCheck(HClinitCheck* instruction) OVERRIDE;
  virtual void VisitCompare(HCompare* instruction) OVERRIDE;
  virtual void VisitCondition(HCondition* instruction) OVERRIDE;
  virtual void VisitConstant(HConstant* instruction) OVERRIDE;
  virtual void VisitDeoptimize(HDeoptimize* instruction) OVERRIDE;
  virtual void VisitDiv(HDiv* instruction) OVERRIDE;
  virtual void VisitDivZeroCheck(HDivZeroCheck* instruction) OVERRIDE;
  virtual void VisitDoubleConstant(HDoubleConstant* instruction) OVERRIDE;
  virtual void VisitEqual(HEqual* instruction) OVERRIDE;
  virtual void VisitExit(HExit* instruction) OVERRIDE;
  virtual void VisitFloatConstant(HFloatConstant* instruction) OVERRIDE;
  virtual void VisitGoto(HGoto* instruction) OVERRIDE;
  virtual void VisitGreaterThan(HGreaterThan* instruction) OVERRIDE;
  virtual void VisitGreaterThanOrEqual(HGreaterThanOrEqual* instruction) OVERRIDE;
  virtual void VisitIf(HIf* instruction) OVERRIDE;
  virtual void VisitInstanceFieldGet(HInstanceFieldGet* instruction) OVERRIDE;
  virtual void VisitInstanceFieldSet(HInstanceFieldSet* instruction) OVERRIDE;
  virtual void VisitInstanceOf(HInstanceOf* instruction) OVERRIDE;
  virtual void VisitIntConstant(HIntConstant* instruction) OVERRIDE;
  virtual void VisitInvoke(HInvoke* instruction) OVERRIDE;
  virtual void VisitInvokeInterface(HInvokeInterface* instruction) OVERRIDE;
  virtual void VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) OVERRIDE;
  virtual void VisitInvokeVirtual(HInvokeVirtual* instruction) OVERRIDE;
  virtual void VisitLessThan(HLessThan* instruction) OVERRIDE;
  virtual void VisitLessThanOrEqual(HLessThanOrEqual* instruction) OVERRIDE;
  virtual void VisitLoadClass(HLoadClass* instruction) OVERRIDE;
  virtual void VisitLoadException(HLoadException* instruction) OVERRIDE;
  virtual void VisitLoadString(HLoadString* instruction) OVERRIDE;
  virtual void VisitLongConstant(HLongConstant* instruction) OVERRIDE;
  virtual void VisitMemoryBarrier(HMemoryBarrier* instruction) OVERRIDE;
  virtual void VisitMonitorOperation(HMonitorOperation* instruction) OVERRIDE;
  virtual void VisitMul(HMul* instruction) OVERRIDE;
  virtual void VisitNeg(HNeg* instruction) OVERRIDE;
  virtual void VisitNewArray(HNewArray* instruction) OVERRIDE;
  virtual void VisitNewInstance(HNewInstance* instruction) OVERRIDE;
  virtual void VisitNot(HNot* instruction) OVERRIDE;
  virtual void VisitNotEqual(HNotEqual* instruction) OVERRIDE;
  virtual void VisitNullCheck(HNullCheck* instruction) OVERRIDE;
  virtual void VisitNullConstant(HNullConstant* instruction) OVERRIDE;
  virtual void VisitOr(HOr* instruction) OVERRIDE;
  virtual void VisitParallelMove(HParallelMove* instruction) OVERRIDE;
  virtual void VisitParameterValue(HParameterValue* instruction) OVERRIDE;
  virtual void VisitPhi(HPhi* instruction) OVERRIDE;
  virtual void VisitRem(HRem* instruction) OVERRIDE;
  virtual void VisitReturn(HReturn* instruction) OVERRIDE;
  virtual void VisitReturnVoid(HReturnVoid* instruction) OVERRIDE;
  virtual void VisitShl(HShl* instruction) OVERRIDE;
  virtual void VisitShr(HShr* instruction) OVERRIDE;
  virtual void VisitStaticFieldGet(HStaticFieldGet* instruction) OVERRIDE;
  virtual void VisitStaticFieldSet(HStaticFieldSet* instruction) OVERRIDE;
  virtual void VisitSub(HSub* instruction) OVERRIDE;
  virtual void VisitSuspendCheck(HSuspendCheck* instruction) OVERRIDE;
  virtual void VisitThrow(HThrow* instruction) OVERRIDE;
  virtual void VisitTypeConversion(HTypeConversion* instruction) OVERRIDE;
  virtual void VisitUnaryOperation(HUnaryOperation* instruction) OVERRIDE;
  virtual void VisitUShr(HUShr* instruction) OVERRIDE;
  virtual void VisitXor(HXor* instruction) OVERRIDE;

#ifdef BUILD_MARSHMALLOW
  virtual void VisitLoadLocal(HLoadLocal* instruction) OVERRIDE;
  virtual void VisitLocal(HLocal* instruction) OVERRIDE;
  virtual void VisitStoreLocal(HStoreLocal* instruction) OVERRIDE;
  virtual void VisitTemporary(HTemporary* instruction) OVERRIDE;
#endif
};

}  // namespace art

#endif  // ART_INJECTION_INJECTION_VISITOR_H_
