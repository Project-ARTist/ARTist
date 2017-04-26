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

#include "artist_environment.h"
#include "dex_file.h"

namespace art {

pthread_mutex_t ArtistEnvironment::mutex_lock = PTHREAD_MUTEX_INITIALIZER;

ArtistEnvironment::ArtistEnvironment()
    : method_vtable_indexes()
    , method_dexfile_idx()
    , FIELD_INSTANCE(0)
    , type_idx_code_lib(0)
    , injections()
    , dex_name_number_map()
    , CODE_LIB_DEX_FILE_NAME()
    , dex_files_apk() {}

ArtistEnvironment::ArtistEnvironment(const size_t method_count)
    : method_vtable_indexes(method_count)
    , method_dexfile_idx(method_count)
    , FIELD_INSTANCE(0)
    , type_idx_code_lib(0)
    , injections()
    , dex_name_number_map()
    , CODE_LIB_DEX_FILE_NAME()
    , dex_files_apk() {}

void ArtistEnvironment::LockSetup() {
  pthread_mutex_lock(&mutex_lock);
}

void ArtistEnvironment::UnlockSetup() {
  VLOG(artistd) << "UnlockSetup()" << std::flush;
  pthread_mutex_unlock(&mutex_lock);
  VLOG(artistd) << "UnlockSetup() DONE" << std::flush;
}

std::string ArtistEnvironment::GetCodeLibDexFileName() {
  return this->CODE_LIB_DEX_FILE_NAME;
}

void ArtistEnvironment::SetCodeLibDexFileName(const std::string& code_lib_dex_name) {
  VLOG(artistd) << "SetCodeLibDexFileName() " << code_lib_dex_name;
  this->CODE_LIB_DEX_FILE_NAME = code_lib_dex_name;
}

void ArtistEnvironment::SetCodeLibDexFile(const DexFile* dex_file) {
  VLOG(artistd) << "SetCodeLibDexFile() " << dex_file;
  VLOG(artistd) << "SetCodeLibDexFile() " << dex_file->GetLocation();
  this->CODE_LIB_DEX_FILE = dex_file;
}

const DexFile * ArtistEnvironment::GetCodeLibDexFile() {
  return this->CODE_LIB_DEX_FILE;
}

bool ArtistEnvironment::IsCodeLib(const DexFile& dex_file) {
  // If Strings are equal, thats the codelib
  if (this->CODE_LIB_DEX_FILE_NAME.compare(dex_file.GetLocation()) == 0) {
    return true;
  } else {
    return false;
  }
}

size_t ArtistEnvironment::GetDexFileCountApk() const {
  return dex_file_count_apk;
}

void ArtistEnvironment::SetDexFileCountApk(size_t _dex_file_count_apk) {
  this->dex_file_count_apk = _dex_file_count_apk;
}

size_t ArtistEnvironment::GetDexFileCountClassPath() const {
  return dex_file_count_classpath;
}

void ArtistEnvironment::SetDexFileCountClassPath(size_t _dex_file_count_classpath) {
  this->dex_file_count_classpath = _dex_file_count_classpath;
}

std::vector<const DexFile*> ArtistEnvironment::GetDexFiles() const {
  return dex_files_apk;
}

void ArtistEnvironment::SetDexFiles(const std::vector<const DexFile*>& dex_files) {
  this->dex_files_apk = dex_files;
}

const art::DexFile* ArtistEnvironment::GetDexFile(const std::string& dex_file_name) const {
  for (auto && dex_file : this->dex_files_apk) {
    if (dex_file_name.compare(dex_file->GetLocation()) == 0) {
      // VLOG(artistd) << "ArtistEnvironment::GetDexFile() Found DexFile: " << dex_file << " (" << dex_file_name << ")";
      return dex_file;
    }
  }
  VLOG(artistd) << "ArtistEnvironment::GetDexFile() Not Found: " << dex_file_name;
  return nullptr;
}

}  // namespace art
