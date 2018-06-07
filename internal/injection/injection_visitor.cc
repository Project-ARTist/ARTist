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

#include "optimizing/artist/api/io/verbose_printer.h"
#include "optimizing/artist/api/modules/artist.h"
#include "injection_visitor.h"
#include "optimizing/artist/internal/injection/visitor_keys.h"

namespace art {

HInjectionVisitor::HInjectionVisitor(HInjectionArtist* parent_artist,
                                     HGraph* method_graph)
    : HGraphVisitor(method_graph)
    , artist(parent_artist)
    , graph(method_graph) {
  DCHECK(artist != nullptr);
  DCHECK(graph != nullptr);
  VLOG(artistd) << "HInjectionVisitor() Injections # " << artist->GetInjections().size();
}

void HInjectionVisitor::InjectInstruction(HInstruction* instruction, shared_ptr<const Injection>& injection) {
  DCHECK(instruction != nullptr);
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() instruction: " << instruction << std::flush;
//  VLOG(artist) << "HInjectionVisitor::InjectInstruction() injection:   " << &injection<< std::flush;

  // ParameterFactory for creating primitives / objects
  VLOG(artistd) << "HInjectionVisitor::InjectInstruction() ParameterCount: " << injection->GetParameters().size();
//  VLOG(artist) << "HInjectionVisitor::InjectInstruction() TargetCount:    " << injection.GetInjectionTargets().size();

  int32_t parameter_count = 0;
  for (auto && parameter : injection->GetParameters()) {
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() Parameter #"
                  << parameter_count++
                  << ": "
                  << Parameter::TypeToString(parameter->GetType());
  }

  for (auto && target : injection->GetInjectionTargets()) {
    std::string check_signature;

    auto target_type = target->GetTargetType();

    switch (target_type) {
      case InjectionTarget::METHOD_CALL_BEFORE:
        check_signature = GetInvokedMethod(instruction);
        break;
      case InjectionTarget::METHOD_CALL_AFTER:
        check_signature = GetInvokedMethod(instruction);
        break;
      case InjectionTarget::METHOD_START:
      case InjectionTarget::METHOD_END:
        check_signature = this->artist->GetMethodInfo().GetMethodName(true);
        break;
      case InjectionTarget::NO_INJECTION:
      default:
        VLOG(artistd) << "HInjectionVisitor::InjectInstruction() DONE: InjectionTarget::NONE";
        continue;
    }
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() check_signature:  " << check_signature;

    const std::string TARGET_SIGNATURE = target->GetTargetSignature();
    VLOG(artistd) << "HInjectionVisitor::InjectInstruction() TARGET_SIGNATURE: " << TARGET_SIGNATURE;

    if (TARGET_SIGNATURE == Target::GENERIC_TARGET
        || MethodSignatureContains(check_signature, TARGET_SIGNATURE)) {
      VLOG(artistd) << "HInjectionVisitor::InjectInstruction() GENERIC Signature OK:   "
                    << "TARGET: " << TARGET_SIGNATURE
                    << " | "
                    << "CHECK: " << check_signature;
      // Inject only if it's not been injected, reuse first injection.
      HInstruction* injection_lib;

      injection_lib = artist->GetCodeLibInstruction();

      std::vector<HInstruction*> function_params;
      function_params.push_back(injection_lib);

      ArtUtils::SetupFunctionParams(graph, injection, function_params);

      HInstruction* injection_location = nullptr;
      if (target_type == InjectionTarget::METHOD_START) {
        injection_location = instruction->GetBlock()->GetGraph()->GetEntryBlock()->GetLastInstruction();
      } else {
        injection_location = instruction;
      }

      bool before = (target_type != InjectionTarget::METHOD_CALL_AFTER);

      ArtUtils::InjectMethodCall(injection_location,
                                 injection->GetSignature(),
                                 function_params,
                                 artist->getCodeLibEnvironment(),
                                 Primitive::Type::kPrimVoid,
                                 before);
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
  }
  VLOG(artistd) << search_string << " NOT start of " << method_signature;
  return false;
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
  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_INVOKE);

  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);

  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() Injections # " << checkInjections.size();

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvoke() DONE";
}

void HInjectionVisitor::VisitInvokeInterface(HInvokeInterface* instruction) {
  DCHECK(instruction != nullptr);
  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_INVOKE_INTERFACE);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeInterface() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);
  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeInterface() DONE";
}

void HInjectionVisitor::VisitInvokeStaticOrDirect(HInvokeStaticOrDirect* instruction) {
  DCHECK(instruction != nullptr);
  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_INVOKE_STATIC_OR_DIRECT);

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

  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_INVOKE_VIRTUAL);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() Injections #" << checkInjections.size()
                << " INVOKED: " << GetInvokedMethod(instruction);

  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() Injections # " << checkInjections.size();

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitInvokeVirtual() DONE";
}

void HInjectionVisitor::VisitReturn(HReturn* instruction) {
  DCHECK(instruction != nullptr);
  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_RETURN);
  VLOG(artistd) << "HInjectionVisitor::VisitReturn() Injections #" << checkInjections.size()
                << " PARENT: " << this->artist->GetMethodInfo().GetMethodName(true);

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitReturn() DONE";
}

void HInjectionVisitor::VisitReturnVoid(HReturnVoid* instruction) {
  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid()" << std::flush;
  DCHECK(instruction != nullptr);

  auto checkInjections = artist->GetInjectionTableEntry(VisitorKeys::H_RETURN_VOID);

  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid() Injections #" << checkInjections.size()
                << " PARENT: " << this->artist->GetMethodInfo().GetMethodName(true);

  for (auto && injection : checkInjections) {
    this->InjectInstruction(instruction, injection);
  }
  VLOG(artistd) << "HInjectionVisitor::VisitReturnVoid() DONE";
}

}  // namespace art
