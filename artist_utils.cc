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
#include "mirror/dex_cache-inl.h"
#include "param_finder.h"
#include "error_handler.h"

#include "injection/primitives.h"
#include "injection/boolean.h"
#include "injection/char.h"
#include "injection/double.h"
#include "injection/float.h"
#include "injection/integer.h"
#include "injection/long.h"

#include "driver/compiler_driver.h"
#include "driver/compiler_driver-inl.h"

using std::static_pointer_cast;

namespace art {

const DexFile::MethodId* ArtUtils::FindMethodId(const HGraph* graph,
                                                const string& searched_method_name) {
  return ArtUtils::FindMethodId(graph->GetDexFile(), searched_method_name);
}

const DexFile::MethodId* ArtUtils::FindMethodId(const DexFile& dex_file,
                                                const string& searched_method_name) {
  const size_t methodCount = dex_file.NumMethodIds();

  const DexFile::MethodId* foundMethodId = nullptr;

  for (uint32_t i = 0; i< methodCount; i++) {
    const DexFile::MethodId& methodId = dex_file.GetMethodId(i);
    const string methodClass = dex_file.GetMethodDeclaringClassDescriptor(methodId);
    const string methodName = dex_file.GetMethodName(methodId);
    const string methodSignature = dex_file.GetMethodSignature(methodId).ToString();
    const string fullyQualifiedMethodName(methodClass + methodName + methodSignature);

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

    MethodIdx ArtUtils::FindMethodIdx(const HGraph* graph, const string& searched_method_name) {
  return FindMethodIdx(graph->GetDexFile(), searched_method_name);
}
MethodIdx ArtUtils::FindMethodIdx(const DexFile& dex_file, const string& searched_method_name) {
  const DexFile::MethodId* methodId = ArtUtils::FindMethodId(dex_file, searched_method_name);
  if (methodId != nullptr) {
    return dex_file.GetIndexForMethodId(*methodId);
  }
  auto msg("Could not find method idx for " + searched_method_name);
  ErrorHandler::abortCompilation(msg);
}

TypeIdx ArtUtils::FindTypeIdxFromName(const HGraph* graph, const string & searched_type_name) {
  return FindTypeIdxFromName(graph->GetDexFile(), searched_type_name);
}

TypeIdx ArtUtils::FindTypeIdxFromName(const DexFile& dex_file, const string & searched_type_name) {
  for (uint32_t i = 0; i < dex_file.NumTypeIds(); i++) {
#ifdef BUILD_OREO
    const DexFile::TypeId& typeId = dex_file.GetTypeId(dex::TypeIndex(i));
#else
    const DexFile::TypeId& typeId = dex_file.GetTypeId(i);
#endif
    string type_name(dex_file.GetTypeDescriptor(typeId));
    if (type_name.compare(searched_type_name) == 0) {
        auto typeIdx = dex_file.GetIndexForTypeId(typeId);
        VLOG(artistd) << " Returning TypeIdx: " << typeIdx << " Type: " << searched_type_name;
#ifdef BUILD_OREO
        return typeIdx.index_;
#else
        return typeIdx;
#endif
    }
  }
  auto msg("Could not find type" + searched_type_name);
  ErrorHandler::abortCompilation(msg);
}

FieldIdx ArtUtils::FindFieldIdxFromName(const HGraph* graph, const string & searched_field_name) {
  return FindFieldIdxFromName(graph->GetDexFile(), searched_field_name);
}

FieldIdx ArtUtils::FindFieldIdxFromName(const DexFile& dex_file, const string & searched_field_type) {
  for (uint32_t i = 0; i < dex_file.NumFieldIds(); i++) {
    const DexFile::FieldId &fieldId = dex_file.GetFieldId(i);

    const string fieldName(dex_file.GetFieldName(fieldId));
    const string fieldType(dex_file.GetFieldTypeDescriptor(fieldId));
    const string fullyQualifiedFieldName(fieldType + fieldName);

    if (fullyQualifiedFieldName.compare(searched_field_type) == 0) {
        FieldIdx fieldIdx = dex_file.GetIndexForFieldId(fieldId);
        VLOG(artistd) << "Found FieldIdx: " << fieldIdx << " for field type " << searched_field_type;
        return fieldIdx;
    }
  }
  auto msg("Could not find type " + searched_field_type);
  ErrorHandler::abortCompilation(msg);
}

ClassDefIdx ArtUtils::FindClassDefIdxFromName(const HGraph* graph, const  string & searched_class_name) {
  return FindClassDefIdxFromName(graph->GetDexFile(), searched_class_name);
}

ClassDefIdx ArtUtils::FindClassDefIdxFromName(const DexFile& dex_file, const  string & searched_class_name) {
  for (uint16_t idx = 0; idx < dex_file.NumClassDefs(); idx++) {
    const DexFile::ClassDef& def = dex_file.GetClassDef(idx);
    // get type
    string class_name = dex_file.GetTypeDescriptor(dex_file.GetTypeId(def.class_idx_));
    if (class_name.find(searched_class_name) != string::npos) {
      VLOG(artistd) << "Found ClassDefId: " << idx << " for class: " << searched_class_name;
      return idx;
    }
  }
  auto msg("Could not find ClassDefId for class: " + searched_class_name);
  ErrorHandler::abortCompilation(msg);
}

void ArtUtils::DumpTypes(const HGraph* graph) {
  ArtUtils::DumpTypes(graph->GetDexFile());
}

void ArtUtils::DumpTypes(const DexFile& dex_file) {
  VLOG(artistd) << "DumpTypes()";

  for (uint32_t i = 0; i < dex_file.NumTypeIds(); i++) {
#ifdef BUILD_OREO
    const DexFile::TypeId& typeID = dex_file.GetTypeId(dex::TypeIndex(i));
#else
    const DexFile::TypeId& typeID = dex_file.GetTypeId(i);
#endif
    string type_name(dex_file.GetTypeDescriptor(typeID));

    VLOG(artistd) << type_name;
  }
}

void ArtUtils::DumpFields(const HGraph* graph) {
  DumpFields(graph->GetDexFile());
}

void ArtUtils::DumpFields(const DexFile& dex_file) {
  VLOG(artistd) << "DumpFields()";

  for (uint32_t i = 0; i < dex_file.NumFieldIds(); i++) {
    const DexFile::FieldId& fieldId = dex_file.GetFieldId(i);
    const string fieldName(dex_file.GetFieldName(fieldId));
    const string typeName(dex_file.GetFieldTypeDescriptor(fieldId));

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
                                      shared_ptr<CodeLibEnvironment> env,
#ifdef BUILD_OREO
                                      const DexCompilationUnit& dex_compilation_unit,
#else
                                      const DexCompilationUnit& dex_compilation_unit ATTRIBUTE_UNUSED,
#endif
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

  auto symbols = env->getCodelibSymbols(&graph->GetDexFile());

  HBasicBlock* injectionBlock = injection_cursor->GetBlock();

  CHECK(injectionBlock != nullptr);

#ifdef BUILD_MARSHMALLOW
  HLoadClass* loadClassCodeLib = new(allocator) HLoadClass(
  symbols->getTypeIdx()
  , false
  , 0);
#elif defined BUILD_OREO
  const DexFile& dex_file = graph->GetDexFile();
  ClassLinker* class_linker = dex_compilation_unit.GetClassLinker();
//  ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
  const DexFile& codelib_dexfile(*env->getDexFile());
//  ArtMethod* art_method = graph->GetArtMethod();
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<1> hs(soa.Self());

  Handle<mirror::ClassLoader> class_loader = dex_compilation_unit.GetClassLoader();

  ArtField* resolved_field = class_linker->ResolveField(dex_file,
                                                        env->getInstanceFieldIdx(),
                                                        dex_compilation_unit.GetDexCache(),
                                                        class_loader,
                                                        false);

  // @CHECK: class_linker_->FindClass(soa.Self(), "Lpackage2/Package2;", class_loader));
  Handle<mirror::Class> codelib_class(hs.NewHandle(resolved_field->GetDeclaringClass()));
  uint32_t dex_pc = 0;
  HLoadClass* loadClassCodeLib = new(allocator) HLoadClass(
      graph->GetCurrentMethod(),
      dex::TypeIndex(symbols->getTypeIdx()),
      graph->GetDexFile(),
      codelib_class,
      false,  /*bool is_referrers_class*/
      dex_pc, /*uint32_t dex_pc*/
      true);  // bool needs_access_check*/  // seems to have no influence, but we also add a manual CLinitcheck in both cases.
#else
  HLoadClass* loadClassCodeLib = new(allocator) HLoadClass(
      graph->GetCurrentMethod(),
      symbols->getTypeIdx(),
      graph->GetDexFile(),
      false,
      0,
      true,    // seems to have no influence, but we also add a manual CLinitcheck in both cases.
      false);  // must be false, crashes otherwise

#endif

  injectionBlock->InsertInstructionBefore(loadClassCodeLib, injection_cursor);

  HClinitCheck* clInitCheckCodelib = new(allocator) HClinitCheck(loadClassCodeLib, 0);
  injectionBlock->InsertInstructionAfter(clInitCheckCodelib, loadClassCodeLib);
  HInstruction* return_cursor = clInitCheckCodelib;

  const bool IS_VOLATILE = false;
  MemberOffset field_offset = env->getInstanceFieldOffset();
#ifdef BUILD_MARSHMALLOW
    HStaticFieldGet* getFieldInstance = new(allocator) HStaticFieldGet(clInitCheckCodelib,
                                                                       Primitive::Type::kPrimNot,
                                                                       field_offset,
                                                                       IS_VOLATILE);
#elif defined BUILD_OREO
  HStaticFieldGet* getFieldInstance = new(allocator) HStaticFieldGet(clInitCheckCodelib,
                                                                     resolved_field,
                                                                     Primitive::Type::kPrimNot,
                                                                     field_offset,
                                                                     IS_VOLATILE,
                                                                     env->getInstanceFieldIdx(),
                                                                     env->getClassDefIdx(),
                                                                     codelib_dexfile,
                                                                     dex_pc);
#else
  // Getting the dexCache;
  // ScopedObjectAccess includes locking the mutator lock
  const DexFile& codelib_dexfile(*env->getDexFile());
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<1> hs(soa.Self());
  ClassLinker* class_linker = Runtime::Current()->GetClassLinker();


  Handle<mirror::DexCache> codelib_dex_cache(
          hs.NewHandle(
                  class_linker->FindDexCache(
                          Thread::Current(), codelib_dexfile, false)));


  FieldIdx field_idx = env->getInstanceFieldIdx();

  HStaticFieldGet* getFieldInstance = new(allocator) HStaticFieldGet(clInitCheckCodelib,
                                                                       Primitive::Type::kPrimNot,
                                                                       field_offset,
                                                                       IS_VOLATILE,
                                                                       field_idx,
                                                                       env->getClassDefIdx(),
                                                                       codelib_dexfile,
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
                                const string& method_signature,
                                vector<HInstruction*>& function_params,
                                shared_ptr<CodeLibEnvironment> env,
                                const Primitive::Type return_type,
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

  const string dex_file_name = ArtUtils::GetDexFileName(graph);

  HBasicBlock* instructionBlock = instruction_cursor->GetBlock();
  HInstruction* firstBlockCursor = graph->GetEntryBlock()->GetLastInstruction();
  ArenaAllocator* allocator = graph->GetArena();

  HBasicBlock* entryBlock = firstBlockCursor->GetBlock();
  CHECK(entryBlock != nullptr);

  const DexFile& current = graph->GetDexFile();
  auto symbols = env->getCodelibSymbols(&current);
  const uint32_t DEX_PC = 0;

  // VTableIndex
#ifdef BUILD_OREO
  Locks::mutator_lock_->SharedLock(Thread::Current());
  ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
  ArtMethod* resolved_method =
      class_linker->ResolveMethod<ClassLinker::ResolveMode::kForceICCECheck>(
          Thread::Current(),
          symbols->getMethodIdx(method_signature),
          graph->GetArtMethod(),
          InvokeType::kVirtual);
  Locks::mutator_lock_->SharedUnlock(Thread::Current());

  HInvokeVirtual* invokeInstruction = new (allocator) HInvokeVirtual(allocator,
                                                                     (uint32_t) function_params.size(),
                                                                     return_type,
                                                                     DEX_PC,
                                                                     symbols->getMethodIdx(method_signature),
                                                                     resolved_method,
                                                                     (uint32_t) env->getMethodVtableIdx(method_signature));
#else
  HInvokeVirtual* invokeInstruction = new (allocator) HInvokeVirtual(allocator,
                                                                     (uint32_t) function_params.size(),
                                                                     return_type,
                                                                     DEX_PC,
                                                                     symbols->getMethodIdx(method_signature),
                                                                     (uint32_t) env->getMethodVtableIdx(method_signature));
#endif
  ArtUtils::SetupInstructionArguments(invokeInstruction, function_params);

  invokeInstruction->SetBlock(instructionBlock);
  if (inject_before) {
    instructionBlock->InsertInstructionBefore(invokeInstruction, instruction_cursor);
  } else {
    instructionBlock->InsertInstructionAfter(invokeInstruction, instruction_cursor);
  }
  VLOG(artistd) << "ArtUtils::InjectMethodCall: " << invokeInstruction;
  VLOG(artist) << "ArtUtils::InjectMethodCall SUCCESS: " << method_signature;

  return invokeInstruction;
}

string ArtUtils::GetMethodName(HInvoke* invoke, bool signature) {
#ifdef BUILD_OREO
  return invoke->GetBlock()->GetGraph()->GetDexFile().PrettyMethod(invoke->GetDexMethodIndex(), signature);
#else
  return PrettyMethod(invoke->GetDexMethodIndex(), invoke->GetBlock()->GetGraph()->GetDexFile(), signature);
#endif
}

string ArtUtils::GetMethodSignature(const HInvoke* invoke) {
  const DexFile& dexfile = invoke->GetBlock()->GetGraph()->GetDexFile();
  auto method_idx = invoke->GetDexMethodIndex();
  const DexFile::MethodId& method_id = dexfile.GetMethodId(method_idx);

  const string method_class = dexfile.GetMethodDeclaringClassDescriptor(method_id);
  const string method_name = dexfile.GetMethodName(method_id);
  const string method_signature = dexfile.GetMethodSignature(method_id).ToString();

  const string fully_qualified_method_name = (method_class + method_name + method_signature);

  return fully_qualified_method_name;
}

string ArtUtils::GetDexFileName(const HGraph* graph) {
  string dex_name = graph->GetDexFile().GetLocation();
  return dex_name;
}

bool ArtUtils::IsNativeMethod(HInvoke* instruction) {
  Locks::mutator_lock_->SharedLock(Thread::Current());
  ClassLinker *class_linker = Runtime::Current()->GetClassLinker();
  HGraph* graph = instruction->GetBlock()->GetGraph();

#ifdef BUILD_MARSHMALLOW
  ArtMethod *resolved_method = class_linker->FindDexCache(
    graph->GetDexFile())->GetResolvedMethod(
      instruction->GetDexMethodIndex(),
      class_linker->GetImagePointerSize());
#elif defined BUILD_OREO
  ArtMethod *resolved_method = class_linker->FindDexCache(
    Thread::Current(),
    graph->GetDexFile())->GetResolvedMethod(
      instruction->GetDexMethodIndex(),
      class_linker->GetImagePointerSize());
#else
  ArtMethod *resolved_method = class_linker->FindDexCache(
    Thread::Current(),
    graph->GetDexFile(),
    false)->GetResolvedMethod(
      instruction->GetDexMethodIndex(),
      class_linker->GetImagePointerSize());
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
void ArtUtils::ExtractMethodArguments(const string &signature, vector<string> &result) {
  string classBuffer = "";
  string paramBuffer = "";

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
        result.push_back(string(paramBuffer + classBuffer));
      } else {
        result.push_back(string(classBuffer));
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
      result.push_back(string(1, character));
      continue;
    } else {
      paramBuffer += character;
      result.push_back(paramBuffer);;
      paramBuffer.clear();
      continue;
    }
  }
}

string ArtUtils::ExtractMethodReturnValue(const string& method_signature) {
  VLOG(artistd) << "ExtractMethodReturnValue() " << method_signature;
  string method_return_value;

  static const string param_end_char = ")";

  size_t pos_param_end = method_signature.find(param_end_char);

  if (pos_param_end == string::npos) {
    // Invalid Method Signature
    return method_return_value;
  }
  // Skip the Closing brace for the return value
  size_t return_value_start = ++pos_param_end;

  method_return_value = method_signature.substr(return_value_start);

  return method_return_value;
}

uint32_t ArtUtils::SetupInstructionArguments(HInvoke* instruction, vector<HInstruction*>& instruction_arguments) {
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
                                   vector<HInstruction*>& function_parameters) {
  VLOG(artistd) << "ArtUtils::SetupFunctionParams()";
  if (injection.GetParameters().size() > 0) {
    VLOG(artistd) << "ArtUtils::SetupFunctionParams() count: " << injection.GetParameters().size();
    for (auto && parameter : injection.GetParameters()) {
      VLOG(artistd) << "ArtUtils::SetupFunctionParams() parameter: " << parameter->GetType();
      switch (parameter->GetType()) {
        case ParameterType::tBoolean: {
          auto paramBoolean = static_pointer_cast<const Boolean>(parameter);
          function_parameters.push_back(graph->GetConstant(Primitive::kPrimBoolean, paramBoolean->GetValue()));
          break;
        }
        case ParameterType::tChar: {
          auto param_char = static_pointer_cast<const Char>(parameter);
          function_parameters.push_back(graph->GetConstant(Primitive::kPrimChar, param_char->GetValue()));
          break;
        }
        case ParameterType::tByte:
        case ParameterType::tShort:
        case ParameterType::tInteger: {
          auto param_integer = static_pointer_cast<const Integer>(parameter);
          function_parameters.push_back(graph->GetIntConstant(param_integer->GetValue()));
          break;
        }
        case ParameterType::tLong: {
          auto param_integer = static_pointer_cast<const Long>(parameter);
          function_parameters.push_back(graph->GetLongConstant(param_integer->GetValue()));
          break;
        }
        case ParameterType::tFloat: {
          auto param_float = static_pointer_cast<const Float>(parameter);
          function_parameters.push_back(graph->GetFloatConstant(param_float->GetValue()));
          break;
        }
        case ParameterType::tDouble: {
          auto param_double = static_pointer_cast<const Double>(parameter);
          function_parameters.push_back(graph->GetDoubleConstant(param_double->GetValue()));
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
#ifdef BUILD_MARSHMALLOW
  Handle<mirror::ClassLoader> class_loader(hs.NewHandle(
      soa.Decode<mirror::ClassLoader*>(compilation_unit.GetClassLoader())));
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(
      compilation_unit.GetClassLinker()->FindDexCache(dex_file)));
#elif defined BUILD_OREO
  Handle<mirror::ClassLoader> class_loader = compilation_unit.GetClassLoader();
  Handle<mirror::DexCache> dex_cache(
    hs.NewHandle(
        compilation_unit.GetClassLinker()->FindDexCache(
            Thread::Current(),
            dex_file)));
#else
  Handle<mirror::ClassLoader> class_loader(
    hs.NewHandle(
      soa.Decode<mirror::ClassLoader*>(compilation_unit.GetClassLoader())));
  Handle<mirror::DexCache> dex_cache(
    hs.NewHandle(
      compilation_unit.GetClassLinker()->FindDexCache(
        Thread::Current(),
        dex_file,
        false)));
#endif
  return driver->ResolveCompilingMethodsClass(soa, dex_cache, class_loader, &compilation_unit);
}

string ArtUtils::GetDexName(const string& dex_name, const uint32_t dex_file_idx) {
  string dex_namey;
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
#ifdef BUILD_OREO
    HEnvironment* newEnvironment = new (allocator) HEnvironment(
        allocator,
        number_of_vregs,
        graph_->GetArtMethod(),
        instruction->GetDexPc(),
        instruction);
#else
    HEnvironment* newEnvironment = new (allocator) HEnvironment(
        allocator,
        number_of_vregs,
        graph_->GetDexFile(),
        graph_->GetMethodIdx(),
        instruction->GetDexPc(),
        graph_->GetInvokeType(),
        instruction);
#endif  // BUILD_OREO
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
