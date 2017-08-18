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

#include "codelib_symbols.h"
#include "optimizing/artist/error_handler.h"

namespace art {

CodelibSymbols::CodelibSymbols(const DexFile* dex_file, shared_ptr<const CodeLib> codelib, jobject jclass_loader)
        : _dex_file(dex_file) {
  // init type index
  _typeIdx = ArtUtils::FindTypeIdxFromName(*dex_file, codelib->getCodeClass());

  ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
#ifdef BUILD_MARSHMALLOW
  // released in destructor
  ReaderMutexLock mu(Thread::Current(), *class_linker->DexLock());
#endif
  // get all the required objects
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<2> hs(soa.Self());
  Handle<mirror::ClassLoader> class_loader(hs.NewHandle(soa.Decode<mirror::ClassLoader*>(jclass_loader)));
#ifdef BUILD_MARSHMALLOW
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(*dex_file)));
#else
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(Thread::Current(), *dex_file, false)));
#endif

  // init method (vtable) indices
  for (MethodSignature signature : codelib->getMethods()) {
      const MethodIdx method_idx = ArtUtils::FindMethodIdx(*dex_file, signature);
      _method_idx[signature] = method_idx;
  }
}


const DexFile* CodelibSymbols::getDexFile() const {
  return _dex_file;
}

TypeIdx CodelibSymbols::getTypeIdx() const {
  return _typeIdx;
}

MethodIdx CodelibSymbols::getMethodIdx(MethodSignature signature) const {
  auto result = _method_idx.find(signature);
  if (result == _method_idx.end()) {
      auto msg("CodelibSymbols: Failed obtaining method idx for signature " + signature);
    ErrorHandler::abortCompilation(msg);
  }
  return result->second;
}

}  // namespace art
