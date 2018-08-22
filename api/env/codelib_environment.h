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
 * @author "Alexander Fink <alexander.fink@cispa.saarland>"
 *
 */

#ifndef ART_API_ENV_CODELIB_ENVIRONMENT_H_
#define ART_API_ENV_CODELIB_ENVIRONMENT_H_

#include <string>
#include <mutex>

#include "codelib_symbols.h"
#include "offsets.h"
#include "optimizing/artist/api/modules/codelib.h"
#include "class_linker.h"
#include "dexfile_environment.h"
#include "artist_typedefs.h"

using std::map;
using std::once_flag;
using std::call_once;
using std::shared_ptr;

namespace art {

class CodelibSymbols;
/**
 * Provides information about a corresponding codelib that is needed to, e.g., inject calls to codelib methods or use
 * its fields. It will initialize all data either in the constructor or lazily when required and then act as a pure
 * data object.
 */
class CodeLibEnvironment {
 public:
  explicit CodeLibEnvironment(shared_ptr<const DexfileEnvironment> dexfile_env, const DexFile* codelib_dex_file,
                              shared_ptr<const CodeLib> codelib, jobject jclass_loader);

  const DexFile* getDexFile() const;
  shared_ptr<const CodelibSymbols> getCodelibSymbols(const DexFile* dex_file) const;

  ClassDefIdx getClassDefIdx() const;
  TypeIdx getTypeIdx() const;
  FieldIdx getInstanceFieldIdx() const;
  MemberOffset getInstanceFieldOffset();
  MethodVtableIdx getMethodVtableIdx(const MethodSignature& signature);

 private:
  MemberOffset findInstanceFieldOffset() const;
  MethodVtableIdx findMethodVtableIdx(const MethodSignature& signature) const;

 private:
  const DexFile* _codelib_dex;
  shared_ptr<const CodeLib> _codelib;

  // initialized in constructor
  ClassDefIdx _cld_idx;
  TypeIdx _type_idx;
  FieldIdx _instance_idx;
  jobject _jclass_loader;
  map<const DexFile*, shared_ptr<CodelibSymbols>> _symbols;
  Handle<mirror::ClassLoader> _class_loader;
  ClassLinker* _class_linker;


  map<const MethodSignature, MethodVtableIdx> methodVtableIdx;

  // lazily initialized
  MemberOffset _instance_offset;

  // lock guard flags for lazily initialized values
  once_flag offset_flag;
};

}  // namespace art

#endif  // ART_API_ENV_CODELIB_ENVIRONMENT_H_
