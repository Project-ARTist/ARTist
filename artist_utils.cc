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

#include <string>

#include "artist_utils.h"
#include "artist_log.h"
#include "env/java_env.h"
#include "env/codelib_environment.h"
#include "mirror/dex_cache.h"
#include "mirror/dex_cache-inl.h"
#include "param_finder.h"

#include "injection/injection.h"

#include "injection/parameter.h"
#include "injection/primitives.h"
#include "injection/boolean.h"
#include "injection/char.h"
#include "injection/double.h"
#include "injection/float.h"
#include "injection/integer.h"
#include "injection/long.h"
#include "injection/object.h"
#include "injection/string.h"

#include "driver/compiler_driver.h"
#include "driver/compiler_driver-inl.h"

namespace art {

const uint32_t ArtUtils::NO_METHOD_IDX_FOUND = 100000;  // DexFile MethodLimit is < 66k
const int64_t ArtUtils::NO_CLASS_DEF_IDX_FOUND = -1;
const int64_t ArtUtils::NO_TYPE_ID_FOUND = -1;
const int64_t ArtUtils::NO_FIELD_ID_FOUND = -1;

const DexFile::MethodId* ArtUtils::FindMethodId(const HGraph* graph,
                                                const std::string& searched_method_name) {
  return ArtUtils::FindMethodId(graph->GetDexFile(), searched_method_name);
}

const DexFile::MethodId* ArtUtils::FindMethodId(const DexFile& dex_file,
                                                const std::string& searched_method_name) {
  const size_t methodCount = dex_file.NumMethodIds();

  const DexFile::MethodId* foundMethodId = nullptr;

  for (uint32_t i = 0; i< methodCount; i++) {
    const DexFile::MethodId& methodId = dex_file.GetMethodId(i);
    const std::string methodClass = dex_file.GetMethodDeclaringClassDescriptor(methodId);
    const std::string methodName = dex_file.GetMethodName(methodId);
    const std::string methodSignature = dex_file.GetMethodSignature(methodId).ToString();
    const std::string fullyQualifiedMethodName(methodClass + methodName + methodSignature);

    if (searched_method_name.compare(fullyQualifiedMethodName) == 0) {
      foundMethodId = &methodId;
      break;
    }
  }
  if (foundMethodId == nullptr) {
    VLOG(artistd) << "NOT FOUND: " << searched_method_name;
  }
  return foundMethodId;
}

int64_t ArtUtils::FindMethodIdx(const HGraph* graph, const std::string& searched_method_name) {
  return FindMethodIdx(graph->GetDexFile(), searched_method_name);
}
int64_t ArtUtils::FindMethodIdx(const DexFile& dex_file, const std::string& searched_method_name) {
  const DexFile::MethodId* methodId = ArtUtils::FindMethodId(dex_file, searched_method_name);

  CHECK(methodId != nullptr);

  uint32_t methodIDx = NO_METHOD_IDX_FOUND;

  if (methodId != nullptr) {
    methodIDx = dex_file.GetIndexForMethodId(*methodId);
  }
  return methodIDx;
}

int64_t ArtUtils::FindTypeIdxFromName(const HGraph* graph, const std::string & searched_type_name) {
  return FindTypeIdxFromName(graph->GetDexFile(), searched_type_name);
}

int64_t ArtUtils::FindTypeIdxFromName(const DexFile& dex_file, const std::string & searched_type_name) {
  int64_t typeIdx = NO_TYPE_ID_FOUND;

  for (uint32_t i = 0; i < dex_file.NumTypeIds(); i++) {
    const DexFile::TypeId& typeId = dex_file.GetTypeId(i);
    std::string type_name(dex_file.GetTypeDescriptor(typeId));
    if (type_name.compare(searched_type_name) == 0) {
      typeIdx = dex_file.GetIndexForTypeId(typeId);
      VLOG(artistd) << "Returning TypeIdx: " << typeIdx << " Type: " << searched_type_name;
    }
  }
  if (typeIdx == NO_TYPE_ID_FOUND) {
    VLOG(artistd) << "Could not find type: " << searched_type_name;
  }
  return typeIdx;
}

int64_t ArtUtils::FindFieldIdxFromName(const HGraph* graph, const std::string & searched_field_name) {
  return FindFieldIdxFromName(graph->GetDexFile(), searched_field_name);
}

int64_t ArtUtils::FindFieldIdxFromName(const DexFile& dex_file, const std::string & searched_field_name) {
  int64_t fieldIdx = NO_FIELD_ID_FOUND;
  for (uint32_t i = 0; i < dex_file.NumFieldIds(); i++) {
    const DexFile::FieldId &fieldId = dex_file.GetFieldId(i);

    const std::string fieldName(dex_file.GetFieldName(fieldId));
    const std::string fieldType(dex_file.GetFieldTypeDescriptor(fieldId));
    const std::string fullyQualifiedFieldName(fieldType + fieldName);

    if (fullyQualifiedFieldName.compare(searched_field_name) == 0) {
      fieldIdx = dex_file.GetIndexForFieldId(fieldId);
      VLOG(artistd) << "Returning FieldIdx: " << fieldIdx << " for Field: " << searched_field_name;
    }
  }
  if (fieldIdx == NO_FIELD_ID_FOUND) {
    VLOG(artistd) << "Could not find type " << searched_field_name;
  }
  return fieldIdx;
}

int64_t ArtUtils::FindClassDefIdxFromName(const HGraph* graph, const  std::string & searched_class_name) {
  return FindClassDefIdxFromName(graph->GetDexFile(), searched_class_name);
}

int64_t ArtUtils::FindClassDefIdxFromName(const DexFile& dex_file, const  std::string & searched_class_name) {
  for (uint16_t i = 0; i < dex_file.NumClassDefs(); i++) {
    const DexFile::ClassDef& def = dex_file.GetClassDef(i);
    // get type
    std::string className = dex_file.GetTypeDescriptor(dex_file.GetTypeId(def.class_idx_));
    if (className.find(searched_class_name) != std::string::npos) {
      VLOG(artistd) << "Found ClassDefId: " << i << " for class: " << searched_class_name;
      return i;
    }
  }
  VLOG(artistd) << "Could not find ClassDefId for class: " << searched_class_name;
  return NO_CLASS_DEF_IDX_FOUND;
}

void ArtUtils::DumpTypes(const HGraph* graph) {
  ArtUtils::DumpTypes(graph->GetDexFile());
}

void ArtUtils::DumpTypes(const DexFile& dex_file) {
  VLOG(artistd) << "DumpTypes()";

  for (unsigned int i = 0; i < dex_file.NumTypeIds(); i++) {
    const DexFile::TypeId& typeID = dex_file.GetTypeId(i);
    std::string type_name(dex_file.GetTypeDescriptor(typeID));

    VLOG(artistd) << type_name;
  }
}

void ArtUtils::DumpFields(const HGraph* graph) {
  DumpFields(graph->GetDexFile());
}

void ArtUtils::DumpFields(const DexFile& dex_file) {
  VLOG(artistd) << "DumpFields()";

  for (unsigned int i = 0; i < dex_file.NumFieldIds(); i++) {
    const DexFile::FieldId& fieldId = dex_file.GetFieldId(i);
    const std::string fieldName(dex_file.GetFieldName(fieldId));
    const std::string typeName(dex_file.GetFieldTypeDescriptor(fieldId));

    VLOG(artistd) << typeName << " : " << fieldName;
  }
}

/** In General we inject the CodeLib object construction once per method we visit.
 *
 *  Subsequent calls use the already injected and initialized CodeLib.
 *
 * @param instruction_cursor cursor where the codelib should get injected
 * @param entry_block_injection true|false Injetc in Entry Block of the graph or tight before the instruction_cursor
 *                              if false is selected.
 *
 */
HInstruction* ArtUtils::InjectCodeLib(const HInstruction* instruction_cursor,
                                      CodeLibEnvironment* env,
                                      const bool entry_block_injection) {
  CHECK(instruction_cursor != nullptr);
  CHECK(env != nullptr);
  VLOG(artistd) << "ArtUtils::InjectCodeLib()" << std::flush;
  HGraph* graph = instruction_cursor->GetBlock()->GetGraph();
  VLOG(artistd) << "ArtUtils::InjectCodeLib() Dex: " << graph->GetDexFile().GetLocation() << std::flush;
  ArenaAllocator* allocator = graph->GetArena();

  HInstruction* injection_cursor;
  if (entry_block_injection) {
    injection_cursor = graph->GetEntryBlock()->GetLastInstruction();
  } else {
    injection_cursor = const_cast<HInstruction*>(instruction_cursor);
  }

  const std::string dex_file_name = ArtUtils::GetDexFileName(graph);


  HBasicBlock* injectionBlock = injection_cursor->GetBlock();

  CHECK(injectionBlock != nullptr);

#ifdef BUILD_MARSHMALLOW

  HLoadClass* loadClassCodeLib = new(allocator) HLoadClass(
  codeLib.GetTypeIdxCodeLib(dex_file_name)
  , false
  , 0);

#else
  HLoadClass* loadClassCodeLib = new(allocator) HLoadClass(
      graph->GetCurrentMethod(),
      env->GetTypeIdxCodeLib(dex_file_name),
      graph->GetDexFile(),
      false,
      0,
      true,    // seems to be of no influence, but we also add a manual CLinitcheck in both cases.
      false);  // must be false, crashes otherwise

#endif
  injectionBlock->InsertInstructionBefore(loadClassCodeLib, injection_cursor);

  HClinitCheck* clInitCheckCodelib = new(allocator) HClinitCheck(loadClassCodeLib, 0);
  injectionBlock->InsertInstructionAfter(clInitCheckCodelib, loadClassCodeLib);
  HInstruction* return_cursor = clInitCheckCodelib;

  #ifdef BUILD_MARSHMALLOW
    const bool IS_VOLATILE = false;
    HStaticFieldGet* getFieldInstance = new(allocator) HStaticFieldGet(clInitCheckCodelib,
                                                                       Primitive::Type::kPrimNot,
                                                                       codeLib.GetInstanceField(codeLib.GetCodeLibDexFileName()),
                                                                       IS_VOLATILE);
  #else
    const bool IS_VOLATILE = false;
    // Getting the dexCache;
    // ScopedObjectAccess include locking the mutator lock
    ScopedObjectAccess soa(Thread::Current());
    StackHandleScope<1> hs(soa.Self());
    ClassLinker* class_linker = Runtime::Current()->GetClassLinker();

    const string& code_lib_dexname = env->GetCodeLibDexFileName();

    Handle<mirror::DexCache> codelib_dex_cache(
        hs.NewHandle(
            class_linker->FindDexCache(
                Thread::Current(), *env->GetCodeLibDexFile(), false)));

    HStaticFieldGet* getFieldInstance = new(allocator) HStaticFieldGet(clInitCheckCodelib,
                                                                       Primitive::Type::kPrimNot,
                                                                       env->GetInstanceField(code_lib_dexname),
                                                                       IS_VOLATILE,
                                                                       env->GetInstanceFieldIdx(code_lib_dexname),
                                                                       env->GetClassDefIdxCodeLib(code_lib_dexname),
                                                                       *env->GetDexFile(code_lib_dexname),
                                                                       codelib_dex_cache,
                                                                       0);
  #endif
    VLOG(artistd) << "ArtUtils::InjectCodeLib getFieldInstance: " << getFieldInstance
                  << " Needs Env: " << getFieldInstance->GetEnvironment();
    injectionBlock->InsertInstructionAfter(getFieldInstance, clInitCheckCodelib);

    HNullCheck* nullCheck = new(allocator) HNullCheck(getFieldInstance, 0);
    injectionBlock->InsertInstructionAfter(nullCheck, getFieldInstance);
    return_cursor = nullCheck;

  return return_cursor;
}

HInstruction* ArtUtils::InjectMethodCall(HInstruction* instruction_cursor,
                                const std::string& method_signature,
                                std::vector<HInstruction*>& function_params,
                                CodeLibEnvironment* env,
                                const Primitive::Type returnType,
                                const bool inject_before) {
  CHECK(env != nullptr);
  VLOG(artistd) << "ArtUtils::InjectMethodCall() Params : "
               << function_params.size()
               << " - "
               << method_signature;
  VLOG(artistd) << "ArtUtils::InjectMethodCall() instruction: " << instruction_cursor << std::flush;
  VLOG(artistd) << "ArtUtils::InjectMethodCall() block:       " << instruction_cursor->GetBlock() << std::flush;
  HGraph* graph = instruction_cursor->GetBlock()->GetGraph();
  VLOG(artistd) << "ArtUtils::InjectMethodCall() graph:       " << graph << std::flush;

  const std::string dex_file_name = ArtUtils::GetDexFileName(graph);

  HBasicBlock* instructionBlock = instruction_cursor->GetBlock();
  HInstruction* firstBlockCursor = graph->GetEntryBlock()->GetLastInstruction();
  ArenaAllocator* allocator = graph->GetArena();

  HBasicBlock* entryBlock = firstBlockCursor->GetBlock();
  CHECK(entryBlock != nullptr);

  const uint32_t DEX_PC = 0;

  // VTableIndex
  HInvokeVirtual* invokeInstruction = new (allocator) HInvokeVirtual(allocator,
                                                                     function_params.size(),
                                                                     returnType,
                                                                     DEX_PC,
                                                                     env->GetMethodDexfileIdx(dex_file_name, method_signature),
                                                                     env->GetMethodVtableIdx(env->GetCodeLibDexFileName(), method_signature));

  ArtUtils::SetupInstructionArguments(invokeInstruction, function_params);

  invokeInstruction->SetBlock(instructionBlock);
  if (inject_before) {
    instructionBlock->InsertInstructionBefore(invokeInstruction, instruction_cursor);
  } else {
    instructionBlock->InsertInstructionAfter(invokeInstruction, instruction_cursor);
  }
  VLOG(artistd) << "ArtUtils::InjectMethodCall: " << invokeInstruction;
  VLOG(artistd) << "ArtUtils::InjectMethodCall SUCCESS: " << method_signature;

  return invokeInstruction;
}

std::string ArtUtils::GetMethodName(HInvoke* invoke, bool signature) {
  return PrettyMethod(invoke->GetDexMethodIndex(), invoke->GetBlock()->GetGraph()->GetDexFile(), signature);
}

std::string ArtUtils::GetMethodSignature(const HInvoke* invoke) {
  const DexFile& dexfile = invoke->GetBlock()->GetGraph()->GetDexFile();
  auto method_idx = invoke->GetDexMethodIndex();
  const DexFile::MethodId& method_id = dexfile.GetMethodId(method_idx);

  const std::string method_class = dexfile.GetMethodDeclaringClassDescriptor(method_id);
  const std::string method_name = dexfile.GetMethodName(method_id);
  const std::string method_signature = dexfile.GetMethodSignature(method_id).ToString();

  const std::string fully_qualified_method_name = (method_class + method_name + method_signature);

  return fully_qualified_method_name;
}

std::string ArtUtils::GetDexFileName(const HGraph* graph) {
  std::string dex_name = graph->GetDexFile().GetLocation();
  return dex_name;
}

bool ArtUtils::IsNativeMethod(HInvoke* instruction) {
  Locks::mutator_lock_->SharedLock(Thread::Current());
  ClassLinker *class_linker = Runtime::Current()->GetClassLinker();
  HGraph* graph = instruction->GetBlock()->GetGraph();

#ifdef BUILD_MARSHMALLOW
  ArtMethod *resolved_method = class_linker->FindDexCache(graph->GetDexFile())->GetResolvedMethod(
      instruction->GetDexMethodIndex(), class_linker->GetImagePointerSize());
#else
  ArtMethod *resolved_method = class_linker->FindDexCache(Thread::Current(), graph->GetDexFile(), false)->GetResolvedMethod(
      instruction->GetDexMethodIndex(), class_linker->GetImagePointerSize());
#endif
  bool result = resolved_method->IsNative();
  Locks::mutator_lock_->SharedUnlock(Thread::Current());

  return result;
}

/** Java Method Param Parser.
 *
 * Parses Java Method signatures for method paramerters in Java Class File Format
 *
 * https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 * BaseType:
 *     B byte
 *     C char
 *     D double
 *     F float
 *     I int
 *     J long
 *     S short
 *     Z boolean
 *
 * ObjectType:
 *     L <ClassName> ;
 *
 * ArrayType:
 *     [ ComponentType
 */
void ArtUtils::ExtractMethodArguments(const std::string &signature, vector<string> &result) {
  std::string classBuffer = "";
  std::string paramBuffer = "";

  bool parsingStarted = false;

  for (auto && character : signature) {
    if (character == JavaEnvironment::FUNCTION_PARAM_START) {
      parsingStarted = true;
      continue;
    }
    if (!parsingStarted) {
      continue;
    }
    if (character == JavaEnvironment::FUNCTION_PARAM_END) {
      break;
    }
    if (character == JavaEnvironment::CLASS) {
      classBuffer += character;
      continue;
    }
    if (character == JavaEnvironment::ARRAY) {
      paramBuffer += character;
      continue;
    }
    if (character == JavaEnvironment::CLASS_TERMINATOR) {
      classBuffer += character;

      if (!paramBuffer.empty()) {
        result.push_back(std::string(paramBuffer + classBuffer));
      } else {
        result.push_back(std::string(classBuffer));
      }
      paramBuffer.clear();
      classBuffer.clear();
      continue;
    }
    // Array BaseType
    if (!classBuffer.empty()) {
      classBuffer += character;
      continue;
    }

    if (paramBuffer.empty()) {
      result.push_back(std::string(1, character));
      continue;
    } else {
      paramBuffer += character;
      result.push_back(paramBuffer);;
      paramBuffer.clear();
      continue;
    }
  }
}

std::string ArtUtils::ExtractMethodReturnValue(const string& method_signature) {
  VLOG(artistd) << "ExtractMethodReturnValue() " << method_signature;
  std::string method_return_value;

  static const std::string param_end_char = ")";

  size_t pos_param_end = method_signature.find(param_end_char);

  if (pos_param_end == std::string::npos) {
    // Invalid Method Signature
    return method_return_value;
  }
  // Skip the Closing brace for the return value
  size_t return_value_start = ++pos_param_end;

  method_return_value = method_signature.substr(return_value_start);

  return method_return_value;
}

uint32_t ArtUtils::SetupInstructionArguments(HInvoke* instruction, std::vector<HInstruction*>& instruction_arguments) {
DCHECK(instruction != nullptr);

uint32_t argument_position = 0;
for (auto const& argument : instruction_arguments) {
  instruction->SetArgumentAt(argument_position, argument);
  ++argument_position;
}
return argument_position;
}

void ArtUtils::SetupFunctionParams(HGraph* graph,
                                   const Injection& injection,
                                   std::vector<HInstruction*>& function_parameters) {
  VLOG(artistd) << "ArtUtils::SetupFunctionParams()";
  if (injection.GetParameters().size() > 0) {
    for (auto && parameter : injection.GetParameters()) {
      switch (parameter->GetType()) {
        case ParameterType::tBoolean: {
          std::shared_ptr<Boolean> paramBoolean = std::static_pointer_cast<Boolean>(parameter);
          paramBoolean->GetType();
          HConstant* constant = graph->GetConstant(Primitive::kPrimBoolean, paramBoolean->GetValue());
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tChar: {
          std::shared_ptr<Char> paramChar = std::static_pointer_cast<Char>(parameter);
          const auto value = paramChar->GetValue();
          HConstant* constant = graph->GetConstant(Primitive::kPrimChar, value);
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tByte:
        case ParameterType::tShort:
        case ParameterType::tInteger: {
          std::shared_ptr<Integer> paramInteger = std::static_pointer_cast<Integer>(parameter);
          const auto value = paramInteger->GetValue();
          HIntConstant* constant = graph->GetIntConstant(value);
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tLong: {
          std::shared_ptr<Long> paramInteger = std::static_pointer_cast<Long>(parameter);
          const auto value = paramInteger->GetValue();
          HLongConstant* constant = graph->GetLongConstant(value);
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tFloat: {
          std::shared_ptr<Float> paramfloat = std::static_pointer_cast<Float>(parameter);
          const auto value = paramfloat->GetValue();
          HFloatConstant* constant = graph->GetFloatConstant(value);
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tDouble: {
          std::shared_ptr<Double> paramDouble = std::static_pointer_cast<Double>(parameter);
          const auto value = paramDouble->GetValue();
          auto constant = graph->GetDoubleConstant(value);
          function_parameters.push_back(constant);
          break;
        }
        case ParameterType::tObject:
        case ParameterType::tString: {
          VLOG(artist) << "ArtUtils::SetupFunctionParams() "
                        << parameter->PrettyName()
                        << " is not supported";
          break;
        }
        case ParameterType::tPrimitive:
        case ParameterType::tParameter: {
          // Impossible
          VLOG(artist) << "ERROR! This case should never be triggered! This probably indicates an implementation bug:"
                        << parameter->PrettyName();
          exit(-1);
        }
      }
    }
  }
  VLOG(artistd) << "ArtUtils::SetupFunctionParams() DONE";
}


mirror::Class* ArtUtils::GetClassFrom(CompilerDriver* driver,
                                   const DexCompilationUnit& compilation_unit) {
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<2> hs(soa.Self());
  const DexFile& dex_file = *compilation_unit.GetDexFile();
  Handle<mirror::ClassLoader> class_loader(hs.NewHandle(
      soa.Decode<mirror::ClassLoader*>(compilation_unit.GetClassLoader())));
#ifdef BUILD_MARSHMALLOW
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(
      compilation_unit.GetClassLinker()->FindDexCache(dex_file)));
#else
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(
      compilation_unit.GetClassLinker()->FindDexCache(Thread::Current(), dex_file, false)));

#endif
  return driver->ResolveCompilingMethodsClass(soa, dex_cache, class_loader, &compilation_unit);
}

std::string ArtUtils::GetDexName(const string& dex_name, const uint32_t dex_file_idx) {
  std::string dex_namey;
  if (dex_file_idx == 0) {
    dex_namey = dex_name;
  } else {
    dex_namey = dex_name;
    dex_namey += DexFile::kMultiDexSeparator;
    dex_namey += "classes";
    dex_namey += std::to_string(dex_file_idx + 1);
    dex_namey += ".dex";
  }
  return dex_namey;
}
#ifndef BUILD_MARSHMALLOW
void ArtUtils::AppendInstruction(HInstruction* instruction, HInstruction* previous_instruction) {
  HBasicBlock* current_block_ = instruction->GetBlock();
  current_block_->AddInstruction(instruction);
  InitializeInstruction(instruction, previous_instruction);
}

void ArtUtils::InsertInstructionAtTop(HInstruction* instruction, HInstruction* previous_instruction) {
  HBasicBlock* current_block_ = instruction->GetBlock();
  if (current_block_->GetInstructions().IsEmpty()) {
    current_block_->AddInstruction(instruction);
  } else {
    current_block_->InsertInstructionBefore(instruction, current_block_->GetFirstInstruction());
  }
  InitializeInstruction(instruction, previous_instruction);
}

void ArtUtils::InitializeInstruction(HInstruction* instruction, HInstruction* previous_instruction) {
  DCHECK(instruction != nullptr);
  DCHECK(previous_instruction != nullptr);

  VLOG(artistd) << "ArtUtils::InitializeInstruction() instruction: " << instruction;

  if (instruction->NeedsEnvironment()) {
    VLOG(artistd) << "ArtUtils::InitializeInstruction() NeedsEnvironment";
    HGraph* graph_ = instruction->GetBlock()->GetGraph();
    VLOG(artistd) << "ArtUtils::InitializeInstruction() graph: " << graph_;
    ArenaAllocator* allocator = graph_->GetArena();
    VLOG(artistd) << "ArtUtils::InitializeInstruction() arena: " << allocator;

    VLOG(artistd) << "ArtUtils::InitializeInstruction() instructionEnvironment: " << instruction->GetEnvironment();
    VLOG(artistd) << "ArtUtils::InitializeInstruction() previousInstructionEnvironment: " << previous_instruction->GetEnvironment();

    size_t number_of_vregs;
    if (previous_instruction->GetEnvironment() == nullptr) {
      number_of_vregs = 0;
    } else {
      number_of_vregs = previous_instruction->GetEnvironment()->Size();
    }

    HEnvironment* newEnvironment = new (allocator) HEnvironment(
        allocator,
        number_of_vregs,
        graph_->GetDexFile(),
        graph_->GetMethodIdx(),
        instruction->GetDexPc(),
        graph_->GetInvokeType(),
        instruction);
    VLOG(artistd) << "ArtUtils::InitializeInstruction() newEnvironment: " << newEnvironment;
    if (previous_instruction->GetEnvironment() != nullptr) {
      VLOG(artistd) << "ArtUtils::InitializeInstruction() CopyFrom previousEnvironment: " << previous_instruction->GetEnvironment();
      newEnvironment->CopyFrom(previous_instruction->GetEnvironment());
    }
    VLOG(artistd) << "ArtUtils::InitializeInstruction() SetRawEnvironment";
    instruction->SetRawEnvironment(newEnvironment);
  }
}
#endif
}  // namespace art
