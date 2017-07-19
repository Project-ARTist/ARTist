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

#include <iomanip>

#include "class_linker.h"
#include "class_linker-inl.h"
#include "runtime.h"
#include "driver/dex_compilation_unit.h"
#include "driver/compiler_driver-inl.h"
#include "driver/compiler_driver.h"

#include "codelib_environment.h"
#include "codelib.h"

#include "optimizing/artist/artist_log.h"

namespace art {

// Methods
CodeLibEnvironment& CodeLibEnvironment::GetInstance() {
  static CodeLibEnvironment instance;  // Guaranteed to be Instantiated on First use and destroyed automatically later.
  return instance;
}

CodeLibEnvironment::~CodeLibEnvironment() {
  pthread_mutex_destroy(&mutex_lock);
  delete[] this->env_setup_ready;
}

void CodeLibEnvironment::PreInitializeEnvironmentCodeLib(jobject class_loader,
                                                         const std::vector<const DexFile *>& dex_files) {
  VLOG(artistd) << "PreInitializeEnvironmentCodeLib()";

  CodeLibEnvironment::GetInstance().SetDexFiles(dex_files);

  CodeLibEnvironment& env = CodeLibEnvironment::GetInstance();
  for (auto && dexFile : dex_files) {
    VLOG(artistd) << "PreInitializeEnvironmentCodeLib() DexFile: " << dexFile->GetLocation();
    env.SetupEnvironment(dex_files, dexFile->GetLocation(), *dexFile, class_loader);
    VLOG(artistd) << "PreInitializeEnvironmentCodeLib() DexFile: " << dexFile->GetLocation() << " DONE";
    VLOG(artistd) << "PreInitializeEnvironmentCodeLib() CodeLibDexFile : " << env.GetCodeLibDexFileName();
    if (dexFile->GetLocation().compare(env.GetCodeLibDexFileName()) == 0) {
      VLOG(artistd) << "PreInitializeEnvironmentCodeLib() FOUND CodeLib: " << dexFile
                                                                           << " (" << dexFile->GetLocation() << ")";
      env.SetCodeLibDexFile(dexFile);
    } else {
      VLOG(artistd) << "PreInitializeEnvironmentCodeLib() NOT CODELIB: " << dexFile->GetLocation();
    }
  }
}

/** Sets up the injecton environment, probes methodIds and more.
 *
 */
void CodeLibEnvironment::SetupEnvironment(const std::vector<const DexFile*>& dex_files,
                                          const std::string& dex_name,
                                          const DexFile& dex_file,
                                          jobject jclass_loader) {
  // Checking Once
  LockSetup();

  static std::atomic_flag setup_base_ready = ATOMIC_FLAG_INIT;

  if (!setup_base_ready.test_and_set()) {
    VLOG(artistd) << "SetupEnvironment() Basic";
    // Get all existing dexfiles
    ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
    Locks::mutator_lock_->SharedLock(Thread::Current());
    ReaderMutexLock mu(Thread::Current(), *class_linker->DexLock());
    const size_t dex_file_count_total = class_linker->GetDexCacheCount();
    Locks::mutator_lock_->SharedUnlock(Thread::Current());

    const std::vector<const DexFile*> dexfiles_classpath = Runtime::Current()->GetClassLinker()->GetBootClassPath();
    this->SetDexFileCountClassPath(dexfiles_classpath.size());

    const size_t boot_classpath_dex_file_count = dexfiles_classpath.size();
    const size_t dexfile_count_apk = dex_file_count_total - boot_classpath_dex_file_count;

    VLOG(artistd) << "CodeLibEnvironment::SetupEnvironment() DexFiles Boot Classpath #" << boot_classpath_dex_file_count;
    VLOG(artistd) << "CodeLibEnvironment::SetupEnvironment() DexFiles Dexcache       #" << dex_file_count_total;

    PreInitializeDexfileEnv(dex_name, dexfiles_classpath, dexfile_count_apk, dex_file_count_total);

    VLOG(artistd) << "SetupEnvironment() Basic DONE";
  } else {
    VLOG(artistd) << "SetupEnvironment() Basic WAS DONE";
  }
  const uint32_t& dex_index = dex_name_number_map.at(dex_name);

  if (!this->env_setup_ready[dex_index].test_and_set()) {
    VLOG(artistd) << "SetupEnvironment() DexFile: " << dex_name << " index: " << dex_index;

    ClassLinker* class_linker = Runtime::Current()->GetClassLinker();

    VLOG(artistd) << "SetupEnvironment(): ArtUtils::FindTypeIdxFromName()";
    const int64_t codelib_type_id =
        ArtUtils::FindTypeIdxFromName(dex_file, CodeLib::_C_CODECLASS);
    if (codelib_type_id == ArtUtils::NO_TYPE_ID_FOUND) {
      ArtUtils::DumpTypes(dex_file);
      VLOG(artist) << "SetupEnvironment() Could not find Codelib DexFile: " << dex_name << " index: " << dex_index;
      VLOG(artist) << "CodeLib is not available";
      this->code_lib_available = false;
      UnlockSetup();
      return;
    }
    const uint32_t code_lib_type_idx = dex_file.GetIndexForTypeId(dex_file.GetTypeId(codelib_type_id));
    VLOG(artistd) << "SetupEnvironment(): ->SetTypeIdxCodeLib()";
    this->SetTypeIdxCodeLib(dex_name, code_lib_type_idx);

    ScopedObjectAccess soa(Thread::Current());
    StackHandleScope<2> hs(soa.Self());
    Handle<mirror::ClassLoader> class_loader(hs.NewHandle(soa.Decode<mirror::ClassLoader*>(jclass_loader)));
#ifdef BUILD_MARSHMALLOW
    Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(dex_file)));
#else
    Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(Thread::Current(), dex_file, false)));
#endif

    VLOG(artistd) << "SetupEnvironment(): ArtUtils::FindFieldIdxFromName()";

    const int64_t codelib_field_instance_idx =
        ArtUtils::FindFieldIdxFromName(dex_file,
                                       CodeLib::_F_CODECLASS_INSTANCE);
    VLOG(artistd) << "SetupEnvironment(): FindFieldIdxFromName() Index: " << codelib_field_instance_idx;
    SetupEnvironmentClassMemberField(dex_name,
                                     dex_file,
                                     class_linker,
                                     soa,
                                     class_loader,
                                     dex_cache,
                                     codelib_field_instance_idx);

    VLOG(artistd) << "Setting up all methods: " << this->GetMethods().size();
    for (auto && METHOD_SIGNATURE : this->GetMethods()) {
      VLOG(artistd) << "SetupEnvironment(): ArtUtils::FindMethodIdx() " << METHOD_SIGNATURE;
      const uint32_t method_idx = ArtUtils::FindMethodIdx(dex_file, METHOD_SIGNATURE);

      if (method_idx == ArtUtils::NO_METHOD_IDX_FOUND) {
        VLOG(artistd) << "Could not find DexFile methodIndex for: " << METHOD_SIGNATURE;
        CHECK(false);
      }

      VLOG(artistd) << "SetupEnvironment(): ->SetMethodDexfileIdx() " << METHOD_SIGNATURE;
      this->SetMethodDexfileIdx(dex_name, METHOD_SIGNATURE, method_idx);

      for (auto && dexy : dex_files) {
        if (dexy->GetLocation().compare(this->GetCodeLibDexFileName()) == 0) {
          SetupEnvironmentMethod(dex_name,
                                 *dexy,
                                 class_linker,
                                 class_loader,
                                 dex_cache,
                                 METHOD_SIGNATURE,
                                 method_idx);
        }
      }
    }
    VLOG(artistd) << "SetupEnvironment() DONE";
    VLOG(artistd) << std::endl;
  } else {
    VLOG(artistd) << "SetupEnvironment() WAS READY DexFile: " << dex_name;
  }
  UnlockSetup();
  VLOG(artistd) << "SetupEnvironment() DONE UnLocked" << std::flush;
}

void CodeLibEnvironment::SetupEnvironmentMethod(const string& dex_name,
                                                const DexFile& dex_file,
                                                ClassLinker* class_linker,
                                                const Handle <mirror::ClassLoader>& class_loader ATTRIBUTE_UNUSED,
                                                Handle <mirror::DexCache>& dex_cache,
                                                const std::string& METHOD_SIGNATURE,
                                                const uint32_t method_idx) {
  VLOG(artistd) << "SetupEnvironment(): ->GetResolvedMethod() " << METHOD_SIGNATURE;

  int64_t dex_file_method_idx = ArtUtils::FindMethodIdx(dex_file, METHOD_SIGNATURE);
  Locks::mutator_lock_->SharedLock(Thread::Current());
  ReaderMutexLock mu(Thread::Current(), *class_linker->DexLock());
  ArtMethod* resolved_method = dex_cache->GetResolvedMethod(dex_file_method_idx, class_linker->GetImagePointerSize());

  VLOG(artistd) << "SetupEnvironment(): resolved_method: " << resolved_method;

  if (resolved_method != nullptr) {
    VLOG(artistd) << "SetupEnvironment(): ->GetVtableIndex() " << METHOD_SIGNATURE;
    const uint32_t method_vtable_idx = resolved_method->GetVtableIndex();

    VLOG(artistd) << "SetupEnvironment(): ->SharedUnlock() resolved_method:" << resolved_method;
    VLOG(artistd) << "SetupEnvironment(): ->SetMethodVtableIdx() " << METHOD_SIGNATURE;
    SetMethodVtableIdx(dex_name, METHOD_SIGNATURE, method_vtable_idx);
    VLOG(artistd) << "DexFileIdx: " << method_idx
                  << " VtableIdx: " << method_vtable_idx
                  << " Method: " << METHOD_SIGNATURE;
  }
  Locks::mutator_lock_->SharedUnlock(Thread::Current());
}

void CodeLibEnvironment::SetupEnvironmentClassMemberField(const string& dex_name,
                                                          const DexFile& dex_file,
                                                          ClassLinker* class_linker,
                                                          const ScopedObjectAccess& soa,
                                                          const Handle <mirror::ClassLoader>& class_loader,
                                                          const Handle <mirror::DexCache>& dex_cache,
                                                          const uint32_t code_lib_field_idx) {
  VLOG(artistd) << "SetupEnvironment(): ArtUtils::FindFieldIdxFromName() instance idx: " << code_lib_field_idx;
  if (code_lib_field_idx == ArtUtils::NO_FIELD_ID_FOUND) {
    ArtUtils::DumpFields(dex_file);
    CHECK(false);
  }
  VLOG(artistd) << "SetupEnvironment(): ->SetInstanceFieldIdx(): " << code_lib_field_idx;
  SetInstanceFieldIdx(dex_name, code_lib_field_idx);
  VLOG(artistd) << "SetupEnvironment(): ResolveField()";
  Locks::mutator_lock_->SharedLock(Thread::Current());
#ifdef BUILD_MARSHMALLOW
  ReaderMutexLock mu(Thread::Current(), *class_linker->DexLock());
#endif
  ArtField* resolved_field = class_linker->ResolveField(dex_file,
                                                        code_lib_field_idx,
                                                        dex_cache,
                                                        class_loader,
                                                        false);
  if (UNLIKELY(resolved_field == nullptr)) {
    // Clean up any exception left by Field resolution.
    soa.Self()->ClearException();
  }
  VLOG(artistd) << "SetupEnvironment(): ResolveField() resolved_field: " << resolved_field;
  if (resolved_field != nullptr) {
    MemberOffset instance_field_offset = resolved_field->GetOffset();
    VLOG(artistd) << "SetupEnvironment(): ->SetInstanceField() instance_field_offset: " << instance_field_offset.SizeValue();
    SetInstanceField(dex_name, instance_field_offset);
  }
  int64_t class_def_idx = ArtUtils::FindClassDefIdxFromName(dex_file, CodeLib::_C_CODECLASS);
  if (class_def_idx != ArtUtils::NO_CLASS_DEF_IDX_FOUND) {
    VLOG(artistd) << "SetupEnvironment(): FindClassDefIdxFromName ClassDef Index: " << class_def_idx;
    SetClassDefIdxCodeLib(dex_name, class_def_idx);
  } else {
    VLOG(artistd) << "SetupEnvironment(): FindClassDefIdxFromName ClassDef Index: " << class_def_idx << " NOT Found";
  }

  Locks::mutator_lock_->SharedUnlock(Thread::Current());
}

/** Initializes the CodeLibEnvironment's member fields, that manages
 *  the DexFile-, Method- and other references.
 *
 * @param dex_name
 * @param dex_files
 * @param dexfile_count_apk
 * @param dex_file_count_total
 */
void CodeLibEnvironment::PreInitializeDexfileEnv(const string& dex_name,
                                                 const vector<const DexFile*>& dex_files,
                                                 const size_t dexfile_count_apk,
                                                 const size_t dex_file_count_total) {
  VLOG(artistd) << "CodeLibEnvironment::PreInitializeDexfileEnv()";

  env_setup_ready = new std::atomic_flag[dex_file_count_total];

  this->SetDexFileCountApk(dexfile_count_apk);
  const uint32_t CODE_LIB_DEX_INDEX = dexfile_count_apk - 1;
  const std::string CODE_LIB_DEX_FILENAME = ArtUtils::GetDexName(dex_name, CODE_LIB_DEX_INDEX);
  this->SetCodeLibDexFileName(CODE_LIB_DEX_FILENAME);

  uint32_t i = 0;
  for (auto && dexFile : dex_files) {
      const string& dex_file_name = dexFile->GetLocation();
      VLOG(artistd) << "#" << std::setw(2) << i << " CodeLibEnvironment::PreInitializeDexfileEnv() BASE DexFile: " << dex_file_name;
      // initialize atomic_flag array #2
      env_setup_ready[i].clear();
      // initialize dexfile_number_map
      dex_name_number_map[dex_file_name] = i;
      ++i;
  }
  VLOG(artistd) << "CodeLibEnvironment::PreInitializeDexfileEnv() DexFiles APK: " << dexfile_count_apk;
  for (uint32_t j = 0; j < dexfile_count_apk; j++) {
      string dex_namey = ArtUtils::GetDexName(dex_name, j);

      env_setup_ready[i].clear();
      // initialize dexfile_number_map
      dex_name_number_map[dex_namey] = i;
      VLOG(artistd) << "#" << std::setw(2) << i << " CodeLibEnvironment::PreInitializeDexfileEnv() DexFile APK: " << dex_namey;
      ++i;
  }
  VLOG(artistd) << "CodeLibEnvironment::PreInitializeDexfileEnv() DONE";
}


CodeLibEnvironment::CodeLibEnvironment()
    : ArtistEnvironment()
    , JAVA_LIB_METHODS(CodeLib::GetMethods()) { }

const std::unordered_set<std::string>& CodeLibEnvironment::GetMethods() const {
  return this->JAVA_LIB_METHODS;
}

const std::vector<std::string> CodeLibEnvironment::GetFields() const {
  return std::vector<std::string>();
}

uint32_t CodeLibEnvironment::GetMethodVtableIdx(const std::string& dex_name,
                                                const std::string& method_signature) const {
  return this->method_vtable_indexes.at(dex_name).at(method_signature);
}

void CodeLibEnvironment::SetMethodVtableIdx(const std::string& dex_name,
                                            const std::string& method_signature,
                                            const uint32_t vtable_idx) {
  this->method_vtable_indexes[dex_name][method_signature] = vtable_idx;
}

const MemberOffset CodeLibEnvironment::GetInstanceField(const std::string& dex_name) const {
  return MemberOffset(this->FIELD_INSTANCE.at(dex_name));
}

void CodeLibEnvironment::SetInstanceField(const std::string& dex_name,
                                          const MemberOffset& class_member_offset) {
  this->FIELD_INSTANCE[dex_name] = class_member_offset.SizeValue();
}

uint32_t CodeLibEnvironment::GetInstanceFieldIdx(const std::string& dex_name) {
  return this->FIELD_INSTANCE_IDX.at(dex_name);
}

void CodeLibEnvironment::SetInstanceFieldIdx(const std::string& dex_name,
                                             const uint32_t instance_field_idx) {
  this->FIELD_INSTANCE_IDX[dex_name] = instance_field_idx;
}

uint32_t CodeLibEnvironment::GetTypeIdxCodeLib(const std::string& dex_name) const {
  return this->type_idx_code_lib.at(dex_name);
}

void CodeLibEnvironment::SetTypeIdxCodeLib(const std::string& dex_name,
                                           const uint32_t class_def_idx_codelib) {
  this->type_idx_code_lib[dex_name] = class_def_idx_codelib;
}

uint32_t CodeLibEnvironment::GetClassDefIdxCodeLib(const std::string& dex_name) const {
  return this->class_def_idx_code_lib.at(dex_name);
}

void CodeLibEnvironment::SetClassDefIdxCodeLib(const std::string& dex_name,
                                               const uint32_t type_idx_codelib) {
  this->class_def_idx_code_lib[dex_name] = type_idx_codelib;
}

uint32_t CodeLibEnvironment::GetMethodDexfileIdx(const std::string& dex_name,
                                                 const std::string& method_signature) const {
  return this->method_dexfile_idx.at(dex_name).at(method_signature);
}

void CodeLibEnvironment::SetMethodDexfileIdx(const std::string& dex_name,
                                             const std::string& method_signature,
                                             const uint32_t method_idx) {
  this->method_dexfile_idx[dex_name][method_signature] = method_idx;
}
}  // namespace art
