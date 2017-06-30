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

#include "analyzer_environment.h"

#include "optimizing/artist/injection/visitor_keys.h"
#include "optimizing/artist/artist_log.h"

namespace art {

// Classes

// Methods

// Methods
AnalyzerEnvironment& AnalyzerEnvironment::GetInstance() {
  static AnalyzerEnvironment instance;  // Guaranteed to be Instantiated on First use and destroyed automatically later.
  return instance;
}

AnalyzerEnvironment::~AnalyzerEnvironment() {
  pthread_mutex_destroy(&mutex_lock);
}

void AnalyzerEnvironment::LockSetup() {
  pthread_mutex_lock(&mutex_lock);
}

void AnalyzerEnvironment::UnlockSetup() {
  pthread_mutex_unlock(&mutex_lock);
}

AnalyzerEnvironment::AnalyzerEnvironment()
    :  ArtistEnvironment()
    , ANALYZER_METHODS() {}

void AnalyzerEnvironment::SetupInjections(std::vector<Injection>& injection_list ATTRIBUTE_UNUSED) {
  VLOG(artistd) << "AnalyzerEnvironment::SetupInjections()";
//  VLOG(artistd) << "AnalyzerEnvironment::SetupInjections() : " << injection_list.size();
  VLOG(artistd) << "AnalyzerEnvironment::SetupInjections() DONE";
}

void AnalyzerEnvironment::SetupEnvironment(const std::vector<const DexFile*>& dex_files ATTRIBUTE_UNUSED,
                                           const std::string& dex_name ATTRIBUTE_UNUSED,
                                           const DexFile& dex_file ATTRIBUTE_UNUSED,
                                           CompilerDriver* compiler_driver ATTRIBUTE_UNUSED,
                                           jobject jclass_loader ATTRIBUTE_UNUSED) { }

const std::unordered_set<std::string>& AnalyzerEnvironment::GetMethods() const {
  return this->ANALYZER_METHODS;
}

const std::vector<std::string> AnalyzerEnvironment::GetFields() const {
  return std::vector<std::string>();
}

uint32_t AnalyzerEnvironment::GetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature) const {
  return this->method_vtable_indexes.at(dex_name).at(method_signature);
}

void AnalyzerEnvironment::SetMethodVtableIdx(const std::string& dex_name, const std::string& method_signature, uint32_t vtable_idx) {
  this->method_vtable_indexes[dex_name][method_signature] = vtable_idx;
}

const MemberOffset AnalyzerEnvironment::GetInstanceField(const std::string& dex_name) const {
  return MemberOffset(this->FIELD_INSTANCE.at(dex_name));
}

void AnalyzerEnvironment::SetInstanceField(const std::string& dex_name, const MemberOffset& class_member_offset) {
  this->FIELD_INSTANCE[dex_name] = class_member_offset.SizeValue();
}

uint32_t AnalyzerEnvironment::GetInstanceFieldIdx(const std::string& dex_name) {
  return this->FIELD_INSTANCE_IDX.at(dex_name);
}

void AnalyzerEnvironment::SetInstanceFieldIdx(const std::string& dex_name, const uint32_t instance_field_idx) {
  this->FIELD_INSTANCE_IDX[dex_name] = instance_field_idx;
}

uint32_t AnalyzerEnvironment::GetTypeIdxCodeLib(const std::string& dex_name) const {
  return this->type_idx_code_lib.at(dex_name);
}

void AnalyzerEnvironment::SetTypeIdxCodeLib(const std::string& dex_name, const uint32_t type_idx_codelib) {
  this->type_idx_code_lib.emplace(dex_name, type_idx_codelib);
}

uint32_t AnalyzerEnvironment::GetClassDefIdxCodeLib(const std::string& dex_name) const {
  return this->class_def_idx_code_lib.at(dex_name);
}

void AnalyzerEnvironment::SetClassDefIdxCodeLib(const std::string& dex_name, const uint32_t type_idx_codelib) {
  this->class_def_idx_code_lib[dex_name] = type_idx_codelib;
}

uint32_t AnalyzerEnvironment::GetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature) const {
  return this->method_dexfile_idx.at(dex_name).at(method_signature);
}

void AnalyzerEnvironment::SetMethodDexfileIdx(const std::string& dex_name, const std::string& method_signature, uint32_t method_idx) {
  this->method_dexfile_idx[dex_name][method_signature] = method_idx;
}

const std::vector<Injection>& AnalyzerEnvironment::GetInjections() {
  VLOG(artistd) << "AnalyzerEnvironment::GetInjections()";
  return this->injections;
}

void AnalyzerEnvironment::AddInjection(const Injection& injection) {
  this->injections.push_back(injection);
}

void AnalyzerEnvironment::SetInjections(const std::vector<Injection>& injection_list) {
  this->injections = injection_list;
}

const std::unordered_map<std::string, std::vector<Injection>>& AnalyzerEnvironment::GetInjectionTable() {
  return this->injection_table;
}

const std::vector<Injection> AnalyzerEnvironment::GetInjectionTableEntry(const std::string& callback_key ATTRIBUTE_UNUSED) {
  std::vector<Injection> injectVector;
  return injectVector;
}

bool AnalyzerEnvironment::EmplaceTableEntry(const std::string& callback_key,
                                            const Injection& single_injection) {
  callback_key.size();
  single_injection.GetSignature();
  return true;
}

}  // namespace art
