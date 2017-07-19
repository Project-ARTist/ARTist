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

#ifndef ART_ENV_ARTIST_ENVIRONMENT_H_
#define ART_ENV_ARTIST_ENVIRONMENT_H_

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <atomic>

#include "offsets.h"
#include "optimizing/artist/injection/injection.h"

#include "jni.h"

namespace art {

class HGraph;
class DexFile;
class CompilerDriver;

class ArtistEnvironment {
 public:
  virtual const std::unordered_set<std::string>& GetMethods() const = 0;

  virtual const std::vector<std::string> GetFields() const = 0;

  virtual uint32_t GetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature) const = 0;
  virtual void SetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature, uint32_t vtable_idx) = 0;

  virtual uint32_t GetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature) const = 0;
  virtual void SetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature, uint32_t method_idx) = 0;

  virtual const MemberOffset GetInstanceField(const std::string& dex_name) const = 0;
  virtual void SetInstanceField(const std::string& dex_name, const MemberOffset& class_member_offset) = 0;

  virtual uint32_t GetInstanceFieldIdx(const std::string& dex_name) = 0;
  virtual void SetInstanceFieldIdx(const std::string& dex_name, const uint32_t instance_field_idx) = 0;

  virtual uint32_t GetTypeIdxCodeLib(const std::string& dex_name) const = 0;
  virtual void SetTypeIdxCodeLib(const std::string& dex_name, uint32_t type_idx_codeLib) = 0;

  virtual uint32_t GetClassDefIdxCodeLib(const std::string& dex_name) const = 0;
  virtual void SetClassDefIdxCodeLib(const std::string& dex_name, const uint32_t type_idx_code_lib) = 0;


  virtual void SetupEnvironment(const std::vector<const DexFile*>& dex_files,
                                const std::string& dex_name,
                                const DexFile& dex_file,
                                jobject jclass_loader) = 0;

 public:
  virtual std::string GetCodeLibDexFileName();
  virtual void SetCodeLibDexFileName(const std::string& code_lib_dex_name);

  virtual void SetCodeLibDexFile(const DexFile* dex_file);
  virtual const DexFile * GetCodeLibDexFile();

  virtual bool IsCodeLib(const DexFile& dex_file);

  virtual bool IsCodeLibAvailable();

  void LockSetup();
  void UnlockSetup();

  ArtistEnvironment();
  explicit ArtistEnvironment(const size_t method_count);

  virtual ~ArtistEnvironment() {}

  size_t GetDexFileCountApk() const;
  void SetDexFileCountApk(size_t _dex_file_count_apk);

  size_t GetDexFileCountClassPath() const;
  void SetDexFileCountClassPath(size_t dex_file_count_classpath);

  std::vector<const DexFile*> GetDexFiles() const;
  void SetDexFiles(const std::vector<const DexFile*>& dex_files);

  const art::DexFile* GetDexFile(const std::string& dex_file_name) const;

 protected:
  std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> method_vtable_indexes;
  std::unordered_map<std::string, std::unordered_map<std::string, uint32_t>> method_dexfile_idx;

  // Static Initialized Class MemberField "instance"
  // Value ist art::MemberOffset
  std::unordered_map<std::string, uint32_t> FIELD_INSTANCE;
  std::unordered_map<std::string, uint32_t> FIELD_INSTANCE_IDX;
  // Type Index of the CodeLib class itself
  std::unordered_map<std::string, uint32_t> type_idx_code_lib;
  // Class Def Index of the CodeLib class itself
  std::unordered_map<std::string, uint32_t> class_def_idx_code_lib;

  std::atomic_flag* env_setup_ready;
  std::unordered_map<std::string, uint32_t> dex_name_number_map;

  static pthread_mutex_t mutex_lock;

  /** By convention this should be the last DexFile in the Class if there are multiple DexFiles */
  std::string CODE_LIB_DEX_FILE_NAME;
  const DexFile* CODE_LIB_DEX_FILE;

  std::vector<const DexFile *> dex_files_apk;

  size_t dex_file_count_apk;
  size_t dex_file_count_classpath;

  bool code_lib_available;
};

}  // namespace art

#endif  // ART_ENV_ARTIST_ENVIRONMENT_H_
