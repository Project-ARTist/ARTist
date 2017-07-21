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

#ifndef ART_ENV_CODELIB_ENVIRONMENT_H_
#define ART_ENV_CODELIB_ENVIRONMENT_H_

#include "artist_environment.h"
#include "offsets.h"
#include "codelib.h"
#include "class_linker.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace art {

class MemberOffset;

class CodeLibEnvironment : public ArtistEnvironment {
 public:
  // codelib will be deleted in destructor
  explicit CodeLibEnvironment(const CodeLib* codeLib) : ArtistEnvironment(), _codeLib(codeLib) {}

  ~CodeLibEnvironment() {
    delete this->_codeLib;
  }

  const std::unordered_set<std::string>& GetMethods() const;
  const std::vector<std::string> GetFields() const;

  uint32_t GetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature) const;
  void SetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature, const uint32_t vtable_idx);

  uint32_t GetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature) const;
  void SetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature, const uint32_t method_idx);

  const MemberOffset GetInstanceField(const std::string& dex_name) const;
  void SetInstanceField(const std::string& dex_name, const MemberOffset& class_member_offset);

  uint32_t GetInstanceFieldIdx(const std::string& dex_name);
  void SetInstanceFieldIdx(const std::string& dex_name, const uint32_t  instance_field_idx);

  uint32_t GetTypeIdxCodeLib(const std::string& dex_name) const;
  void SetTypeIdxCodeLib(const std::string& dex_name, const uint32_t class_def_idx_codelib);

  uint32_t GetClassDefIdxCodeLib(const std::string& dex_name) const;
  void SetClassDefIdxCodeLib(const std::string& dex_name, const uint32_t type_idx_codelib);




  // TODO REFACTOR
  static void PreInitializeEnvironmentCodeLib(jobject class_loader,
                                              const std::vector<const DexFile *> &dex_files);
  // TODO REFACTOR
  virtual void SetupEnvironment(const std::vector<const DexFile*>& dex_files,
                                const std::string& dex_name,
                                const DexFile& dex_file,
                                jobject jclass_loader);


 private:
  const CodeLib* _codeLib;

  // TODO REFACTOR
  void PreInitializeDexfileEnv(const std::string& dex_name, const std::vector<const DexFile*>& dex_files,
                               const size_t dexfile_count_apk, const size_t dex_file_count_total);
  // TODO REFACTOR
  void SetupEnvironmentClassMemberField(const std::string& dex_name,
                                        const DexFile& dex_file,
                                        ClassLinker* class_linker,
                                        const ScopedObjectAccess& soa,
                                        // StackHandleScope<2>& hs,
                                        const Handle <mirror::ClassLoader>&,
                                        const Handle <mirror::DexCache>& dex_cache,
                                        const uint32_t code_lib_field_idx);
  // TODO REFACTOR
  void SetupEnvironmentMethod(const std::string& dex_name,
                              const DexFile& dex_file,
                              ClassLinker* class_linker,
                              const Handle <mirror::ClassLoader>& class_loader,
                              Handle <mirror::DexCache>& dex_cache,
                              const std::string& METHOD_SIGNATURE,
                              const uint32_t method_idx);
};

}  // namespace art

#endif  // ART_ENV_CODELIB_ENVIRONMENT_H_
