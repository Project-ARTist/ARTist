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

#include <iomanip>
#include "class_linker.h"

#include "driver/compiler_driver-inl.h"
#include "codelib_environment.h"

#include "optimizing/artist/api/io/artist_log.h"
#include "optimizing/artist/api/utils/artist_utils.h"
#include "optimizing/artist/api/io/error_handler.h"

using std::make_shared;

namespace art {

// codelib will be deleted in destructor
CodeLibEnvironment::CodeLibEnvironment(shared_ptr<const DexfileEnvironment> dexfile_env,
                                       const DexFile *codelib_dex_file, shared_ptr<const CodeLib> codelib,
                                       jobject jclass_loader)
        : _codelib_dex(codelib_dex_file), _codelib(codelib), _jclass_loader(jclass_loader), _instance_offset(art::MemberOffset(0)) {
  for (auto dex_file : dexfile_env->getAppDexFiles()) {
    auto symbols = make_shared<CodelibSymbols>(dex_file, codelib, jclass_loader);
    _symbols[dex_file] = symbols;
  }

  Locks::mutator_lock_->SharedLock(Thread::Current());
  // init _class_linker
  _class_linker = Runtime::Current()->GetClassLinker();

#ifdef BUILD_MARSHMALLOW
  // mutex is released in destructor
  ReaderMutexLock mu(Thread::Current(), *_class_linker->DexLock());
#endif
  // init class loader
  ScopedObjectAccess soa(Thread::Current());
  StackHandleScope<2> hs(soa.Self());
#ifdef BUILD_OREO
  _class_loader = hs.NewHandle(soa.Decode<mirror::ClassLoader>(_jclass_loader));
#else
  _class_loader = hs.NewHandle(soa.Decode<mirror::ClassLoader*>(_jclass_loader));
#endif  // BUILD_OREO

  Locks::mutator_lock_->SharedUnlock(Thread::Current());

  // init class def index
  auto codelib_class = codelib->getCodeClass();
  if (!ArtUtils::FindClassDefIdxFromName(codelib_dex_file, codelib_class, &_cld_idx)) {
    auto msg("Could not find ClassDefId for class: " + codelib_class);
    ErrorHandler::abortCompilation(msg);
  }

  // init type index
//  _type_idx = ArtUtils::FindTypeIdxFromName(*codelib_dex_file, _codelib->getCodeClass());

  if (!ArtUtils::FindTypeIdxFromName(codelib_dex_file, codelib_class, &_type_idx)) {
    auto msg("Could not find type " + codelib_class);
    ErrorHandler::abortCompilation(msg);
  }

  // init singleton instance field index
//  _instance_idx = ArtUtils::FindFieldIdxFromName(*codelib_dex_file, _codelib->getInstanceField());
  auto instance_field = _codelib->getInstanceField();
  if (!ArtUtils::FindFieldIdxFromName(codelib_dex_file, instance_field, &_instance_idx)) {
    auto msg("Could not find type " + instance_field);
    ErrorHandler::abortCompilation(msg);
  }
}

const DexFile* CodeLibEnvironment::getDexFile() const {
  return _codelib_dex;
}

shared_ptr<const CodelibSymbols> CodeLibEnvironment::getCodelibSymbols(const DexFile* dex_file) const {
  auto result = _symbols.find(dex_file);
  if (result == _symbols.end()) {
    return nullptr;
  }
  return result->second;
}

ClassDefIdx CodeLibEnvironment::getClassDefIdx() const {
  return _cld_idx;
}

TypeIdx CodeLibEnvironment::getTypeIdx() const {
  return _type_idx;
}

FieldIdx CodeLibEnvironment::getInstanceFieldIdx() const {
  return _instance_idx;
}

/**
 * Provides the offset to the codelib's static singleton instance field.
 * The offset is initialized lazily a single time so that consecutive calls will function as a pure getter.
 *
 * @return MemberOffset to singleton instance field.
 */
MemberOffset CodeLibEnvironment::getInstanceFieldOffset() {
  // lazy init: executed only by the first thread (others are blocked) and ignored for consecutive calls.
  call_once(offset_flag, [this]() {this->_instance_offset = this->findInstanceFieldOffset();});

  // after the initial setup, this function simply returns the already computed offset
  return _instance_offset;
}

/**
 * Provides the index to the codelib's vtable for a given method signature.
 *
 * @return vtable index for the given signature.
 */
MethodVtableIdx CodeLibEnvironment::getMethodVtableIdx(const MethodSignature& signature) {
  if (methodVtableIdx.find(signature) == methodVtableIdx.end()) {
    methodVtableIdx[signature] = this->findMethodVtableIdx(signature);
  }
  return methodVtableIdx[signature];
}

/**
 * Finds the offset of the codelib's static singleton instance field.
 *
 * @return MemberOffset to the codelib's singleton field.
 */
MemberOffset CodeLibEnvironment::findInstanceFieldOffset() const {
  // init dex file cache
  Locks::mutator_lock_->SharedLock(Thread::Current());
  StackHandleScope<2> hs(Thread::Current());

#ifdef BUILD_MARSHMALLOW
  auto codelib_dex_cache = hs.NewHandle(_class_linker->FindDexCache(*_codelib_dex));
#elif defined BUILD_OREO
  auto codelib_dex_cache = hs.NewHandle(_class_linker->FindDexCache(Thread::Current(), *_codelib_dex));
#else
  auto codelib_dex_cache = hs.NewHandle(_class_linker->FindDexCache(Thread::Current(), *_codelib_dex, false));
#endif

  // init instance offset
  CHECK(_class_linker != nullptr);
  ArtField* resolved_field = _class_linker->ResolveField(*_codelib_dex,
                                                         _instance_idx,
                                                         codelib_dex_cache,
                                                         _class_loader,
                                                         false);
  if (resolved_field == nullptr) {
    auto msg = "Could not resolve codelib instance field for dex file " + _codelib_dex->GetLocation();
    ArtUtils::DumpFields(*_codelib_dex);
    ErrorHandler::abortCompilation(msg);
  } else {
    auto result = resolved_field->GetOffset();
    Locks::mutator_lock_->SharedUnlock(Thread::Current());
    return result;
  }
}

/**
 * Finds the vtable index for a particular codelib function.
 *
 * @return the vtable index for the given signature.
 */
MethodVtableIdx CodeLibEnvironment::findMethodVtableIdx(const MethodSignature& signature) const {
  ScopedObjectAccess soa(Thread::Current());
#ifdef BUILD_MARSHMALLOW
  // released in destructor
  ReaderMutexLock mu(Thread::Current(), *_class_linker->DexLock());
#endif
  // get all the required objects
  ClassLinker* class_linker = Runtime::Current()->GetClassLinker();
  StackHandleScope<2> hs(soa.Self());
#ifdef BUILD_OREO
//  Handle<mirror::ClassLoader> class_loader(hs.NewHandle(soa.Decode<mirror::ClassLoader>(_jclass_loader)));
#else
  Handle<mirror::ClassLoader> class_loader(hs.NewHandle(soa.Decode<mirror::ClassLoader*>(_jclass_loader)));
#endif
#ifdef BUILD_MARSHMALLOW
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(*_codelib_dex)));
#elif defined BUILD_OREO
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(Thread::Current(), *_codelib_dex)));
#else
  Handle<mirror::DexCache> dex_cache(hs.NewHandle(class_linker->FindDexCache(Thread::Current(), *_codelib_dex, false)));
#endif

  MethodIdx method_idx;
  if (!ArtUtils::FindMethodIdx(_codelib_dex, signature, &method_idx)) {
    auto msg("Could not find method idx for " + signature);
    ErrorHandler::abortCompilation(msg);
  }

  ArtMethod* resolved_method = dex_cache->GetResolvedMethod(method_idx, class_linker->GetImagePointerSize());

  size_t index = 0;
  if (resolved_method == nullptr) {
    auto msg = "Could not resolve method " + signature + " for dex file " + _codelib_dex->GetLocation();
    ErrorHandler::abortCompilation(msg);
  } else {
    index = resolved_method->GetVtableIndex();
  }
  return index;
}

}  // namespace art
