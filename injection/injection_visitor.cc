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

#include "optimizing/artist/artist.h"
#include "injection_visitor.h"
#include "optimizing/artist/artist_utils.h"
#include "optimizing/artist/env/codelib_environment.h"
#include "visitor_keys.h"
#include "parameter.h"
#include "optimizing/nodes.h"

namespace art {

HInjectionVisitor::HInjectionVisitor(HArtist* parentArtist,
                                     HGraph* method_graph,
                                     const std::vector<Injection>& injection_list)
    : HArtistMethodVisitor(method_graph)
    , artist(parentArtist)
    , graph(method_graph)
    , code_lib(CodeLibEnvironment::GetInstance())
    , injections(injection_list) {
  DCHECK(artist != nullptr);
  DCHECK(graph != nullptr);
  VLOG(artistd) << "HInjectionVisitor()";
  VLOG(artistd) << "HInjectionVisitor() Injections# " << injections.size();
  VLOG(artistd) << "HInjectionVisitor() DONE";
}

void HInjectionVisitor::InjectInstruction(HInstruction* instruction, const Injection& injection) {
  DCHECK(instruction != nullptr);
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() instruction: " << instruction << std::flush;
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() injection:   " << &injection<< std::flush;

  // ParameterFactory for creating primitives / objects
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() ParameterCount: " << injection.GetParameters().size();
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() TargetCount:    " << injection.GetInjectionTargets().size();

  int32_t parameter_count = 0;
  for (auto && parameter : injection.GetParameters()) {
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() Parameter #"
                  << parameter_count++
                  << ": "
                  << Parameter::TypeToString(parameter->GetType());
  }

  for (auto && target : injection.GetInjectionTargets()) {
    std::string check_signature;

    switch (target.GetTargetType()) {
      case InjectionTarget::METHOD_CALL_BEFORE:
        check_signature = GetInvokedMethod(instruction);
        break;
      case InjectionTarget::METHOD_CALL_AFTER:
        check_signature = GetInvokedMethod(instruction);
        break;
      case InjectionTarget::METHOD_START:
      case InjectionTarget::METHOD_END:
        check_signature = this->artist->GetMethodInfo()->GetMethodName(true);
        break;
      case InjectionTarget::NO_INJECTION:
      default:
        VLOG(artistd) << "HInjectionVisitor::InjectInstruction() DONE: InjectionTarget::NONE";
        continue;
    }
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() check_signature:  " << check_signature;

    const std::string TARGET_SIGNATURE = target.GetTargetSignature();
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() TARGET_SIGNATURE: " << TARGET_SIGNATURE;

    if (TARGET_SIGNATURE == Target::GENERIC_TARGET
        || MethodSignatureContains(check_signature, TARGET_SIGNATURE)) {
      VLOG(artistd) << "HInjectionVisitor::InjectInstruction() GENERIC Signature OK:   "
                    << "TARGET: " << TARGET_SIGNATURE
                    << " | "
                    << "CHECK: " << check_signature;
      // Inject only if it's not been injected, reuse first injection.
      HInstruction* injection_lib;

      injection_lib = artist->GetCodeLib();
      std::vector<HInstruction*> function_params;
      function_params.push_back(injection_lib);

      ArtUtils::SetupFunctionParams(graph, injection, function_params);

      HInstruction* injection_location = nullptr;
      if (target.GetTargetType() == InjectionTarget::METHOD_START) {
        injection_location = instruction->GetBlock()->GetGraph()->GetEntryBlock()->GetLastInstruction();
      } else {
        injection_location = instruction;
      }
      ArtUtils::InjectMethodCall(injection_location,
                                 injection.GetSignature(),
                                 function_params,
                                 Primitive::Type::kPrimVoid,
                                 true);

    } else {
      VLOG(artistd) << "HInjectionVisitor::InjectInstruction() Signature Fail! HAVE:   " << check_signature << std::endl
                    << "HInjectionVisitor::InjectInstruction() Signature Fail! TARGET: " << TARGET_SIGNATURE;
    }
  }
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() DONE";
}

std::string HInjectionVisitor::GetInvokedMethod(HInstruction* instruction) {
  if (instruction->IsInvokeStaticOrDirect()) {
    HInvokeStaticOrDirect* invoke = reinterpret_cast<HInvokeStaticOrDirect*>(instruction);
    invoke->GetDexMethodIndex();
    return ArtUtils::GetMethodSignature(reinterpret_cast<HInvoke*>(instruction));
  } else if (instruction->IsInvokeVirtual()) {
    HInvokeVirtual* invoke = reinterpret_cast<HInvokeVirtual*>(instruction);
    invoke->GetDexMethodIndex();
    return ArtUtils::GetMethodSignature(reinterpret_cast<HInvoke*>(instruction));
  } else if (instruction->IsInvokeInterface()) {
    HInvokeInterface* invoke = reinterpret_cast<HInvokeInterface*>(instruction);
    invoke->GetDexMethodIndex();
    return ArtUtils::GetMethodSignature(reinterpret_cast<HInvoke*>(instruction));
  } else if (instruction->IsInvoke()) {
    HInvoke* invoke = reinterpret_cast<HInvoke*>(instruction);
    invoke->GetDexMethodIndex();
    return ArtUtils::GetMethodSignature(reinterpret_cast<HInvoke*>(instruction));
  }
  return "";
}

/**
 *  Checks if the passed Method signature starts with the search string.
 *
 *  Notation scheme is e.g.:
 *  - `void b.a.a.a.a.b.ad.a(byte[], int[])`,
 *  - `java.io.RandomAccessFile b.a.a.a.a.b.ad.b(java.io.File)`
 *
 *  => see `art::PrettyMethod()` in `<art>/runtime/utils.cc`
 *
 * @param method_signature
 * @param search_string search token, that is looked for in method_signature
 * @return
 */
bool HInjectionVisitor::MethodSignatureStartsWith(const std::string& method_signature,
                                                  const std::string& search_string) {
  if (method_signature.substr(0, search_string.size()) == search_string) {
    VLOG(artistd) << search_string << " is start of " << method_signature << std::endl;
    return true;
  } else {
    VLOG(artistd) << search_string << " NOT start of " << method_signature;
    return false;
  }
}

/**
 *  Checks if the passed Method signature starts with the search string.
 *
 *  Notation scheme is e.g.:
 *  - `void b.a.a.a.a.b.ad.a(byte[], int[])`,
 *  - `java.io.RandomAccessFile b.a.a.a.a.b.ad.b(java.io.File)`
 *
 *  => see `art::PrettyMethod()` in `<art>/runtime/utils.cc`
 *
 * @param method_signature
 * @param search_string search token, that is looked for in method_signature
 *
 * @return
 */
bool HInjectionVisitor::MethodSignatureContains(const std::string& method_signature,
                                                const std::string& search_string) {
  if (method_signature.find(search_string) != std::string::npos) {
    VLOG(artistd) << method_signature << " contains " << search_string << std::endl;
    return true;
  } else {
    VLOG(artistd) << method_signature << " DOES NOT contain " << search_string << std::endl;
    return false;
  }
}

void HInjectionVisitor::VisitInvoke(HInvoke* instruction) {
  DCHECK(instruction != nullptr);
  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_INVOKE);

  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);

  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() Injections# " << checkInjections.size();

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() DONE";
}

void HInjectionVisitor::VisitInvokeInterface(HInvokeInterface* instruction) {
  DCHECK(instruction != nullptr);
  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_INVOKE_INTERFACE);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeInterface() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);
  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeInterface() DONE";
}

void HInjectionVisitor::VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) {
  DCHECK(instruction != nullptr);
  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_INVOKE_STATIC_OR_DIRECT);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeStaticOrDirect() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeStaticOrDirect() DONE";
}

void HInjectionVisitor::VisitInvokeVirtual(HInvokeVirtual* instruction) {
  DCHECK(instruction != nullptr);
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual()";

  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_INVOKE_VIRTUAL);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() Injections# " << checkInjections.size();

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() DONE";
}

void HInjectionVisitor::VisitReturn(HReturn* instruction) {
  DCHECK(instruction != nullptr);
  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_RETURN);

  VLOG(artistd) << "HInjectionVisitor::VisitReturn() Injections #" << checkInjections.size()
                << " PARENT: " << this->artist->GetMethodInfo()->GetMethodName(true);

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitReturn() DONE";
}

void HInjectionVisitor::VisitReturnVoid(HReturnVoid* instruction) {
  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid()" << std::flush;
  DCHECK(instruction != nullptr);

  const std::vector<Injection>& checkInjections = code_lib.GetInjectionTableEntry(VisitorKeys::H_RETURN_VOID);

  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid() Injections #" << checkInjections.size()
                << " PARENT: " << this->artist->GetMethodInfo()->GetMethodName(true);

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid() DONE";
}

void HInjectionVisitor::VisitAdd(HAdd* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitAnd(HAnd* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitArrayGet(HArrayGet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitArrayLength(HArrayLength* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitArraySet(HArraySet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitBinaryOperation(HBinaryOperation* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitBooleanNot(HBooleanNot* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitBoundsCheck(HBoundsCheck* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitBoundType(HBoundType* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitCheckCast(HCheckCast* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitClinitCheck(HClinitCheck* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitCompare(HCompare* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitCondition(HCondition* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitConstant(HConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitDeoptimize(HDeoptimize* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitDiv(HDiv* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitDivZeroCheck(HDivZeroCheck* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitDoubleConstant(HDoubleConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitEqual(HEqual* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitExit(HExit* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitFloatConstant(HFloatConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitGoto(HGoto* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitGreaterThan(HGreaterThan* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitGreaterThanOrEqual(HGreaterThanOrEqual* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitIf(HIf* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitInstanceFieldGet(HInstanceFieldGet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitInstanceFieldSet(HInstanceFieldSet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitInstanceOf(HInstanceOf* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitIntConstant(HIntConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLessThan(HLessThan* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLessThanOrEqual(HLessThanOrEqual* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLoadClass(HLoadClass* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLoadException(HLoadException* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLoadString(HLoadString* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitLongConstant(HLongConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitMemoryBarrier(HMemoryBarrier* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitMonitorOperation(HMonitorOperation* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitMul(HMul* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNeg(HNeg* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNewArray(HNewArray* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNewInstance(HNewInstance* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNot(HNot* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNotEqual(HNotEqual* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNullCheck(HNullCheck* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitNullConstant(HNullConstant* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitOr(HOr* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitParallelMove(HParallelMove* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitParameterValue(HParameterValue* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitPhi(HPhi* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitRem(HRem* instruction) {
  DCHECK(instruction != nullptr);
}


void HInjectionVisitor::VisitShl(HShl* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitShr(HShr* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitStaticFieldGet(HStaticFieldGet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitStaticFieldSet(HStaticFieldSet* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitSub(HSub* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitSuspendCheck(HSuspendCheck* instruction) {
  DCHECK(instruction != nullptr);
}


void HInjectionVisitor::VisitThrow(HThrow* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitTypeConversion(HTypeConversion* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitUnaryOperation(HUnaryOperation* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitUShr(HUShr* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitXor(HXor* instruction) {
  DCHECK(instruction != nullptr);
}

#ifdef BUILD_MARSHMALLOW

void HInjectionVisitor::VisitLoadLocal(HLoadLocal* instruction) {
  DCHECK(instruction != nullptr);
}
void HInjectionVisitor::VisitLocal(HLocal* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitStoreLocal(HStoreLocal* instruction) {
  DCHECK(instruction != nullptr);
}

void HInjectionVisitor::VisitTemporary(HTemporary* instruction) {
  DCHECK(instruction != nullptr);
}
#endif

}  // namespace art
