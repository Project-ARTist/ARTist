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
 *
 */

#ifndef ART_ENV_CODELIB_SYMBOLS_H_
#define ART_ENV_CODELIB_SYMBOLS_H_

#include <map>

#include "class_linker.h"
#include "class_linker-inl.h"
#include "art_method.h"
#include "mirror/class.h"
#include "mirror/dex_cache.h"
#include "scoped_thread_state_change.h"
#include "optimizing/artist/env/codelib_environment.h"
#include "optimizing/artist/artist_utils.h"
#include "optimizing/artist/artist_typedefs.h"
#include "codelib.h"


using std::map;
using std::exception;
using std::shared_ptr;

namespace art {

/**
 * Stores and provides dexfile-specific information about the codelib.
 */
class CodelibSymbols {
 public:
    CodelibSymbols(const DexFile* dex_file, shared_ptr<const CodeLib> codelib, jobject jclass_loader);

    const DexFile* getDexFile() const;

    TypeIdx getTypeIdx() const;
    MethodIdx getMethodIdx(MethodSignature signature) const;


 private:
    const DexFile* _dex_file;

    // the codelib type index in the given dexfile
    TypeIdx _typeIdx;

    // codelib method indices in the given dexfile
    map<MethodSignature, MethodIdx> _method_idx;
};

}  // namespace art

#endif  // ART_ENV_CODELIB_SYMBOLS_H_
