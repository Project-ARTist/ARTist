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

#include "module_manager.h"
#include "optimizing/artist/api/utils/artist_utils.h"
#include "optimizing/artist/api/io/error_handler.h"

namespace art {

ModuleManager& ModuleManager::getInstance() {
  // Automated creation and destruction through `static` modifier.
  static ModuleManager instance;
  return instance;
}

bool ModuleManager::registerModule(ModuleId id, shared_ptr<Module> module) {
  CHECK(module);
  VLOG(artistd) << "ModuleManager: registering module " << id;

  if (_modules.find(id) != _modules.end()) {
      return false;
  }
  _modules[id] = module;
  return true;
}

shared_ptr<Module> ModuleManager::getModule(ModuleId id) const {
  if (_modules.find(id) == _modules.end()) {
      return nullptr;
  }
  return _modules.at(id);
}

shared_ptr<const DexfileEnvironment> ModuleManager::getDexFileEnvironment() const {
  return _dex_file_env;
}

shared_ptr<CodeLibEnvironment> ModuleManager::getCodelibEnvironment(ModuleId id) const {
  CHECK(initialized());
  if (_environments.find(id) == _environments.end()) {
      // some modules do not have a codelib, hence no corresponding environment
      VLOG(artistd) << "ModuleManager: could not find codelib env for module " + id;
      return nullptr;
  }
  VLOG(artistd) << "ModuleManager: returning codelib env for module " + id;
  return _environments.at(id);
}

const map<ModuleId, shared_ptr<Module>> ModuleManager::getModules() const {
  VLOG(artistd) << "ModuleManager: obtaining the modules map with " << _modules.size() << " entries.";
  return _modules;
}

void ModuleManager::initializeModules(vector<const DexFile*> dex_files, jobject jclass_loader) {
  VLOG(artistd) << "ModuleManager: initializing modules ";
  if (_init_flag.exchange(true)) {  // atomic check and set
      string msg = "ModuleManager: Attempting to initialize modules more than once.";
    ErrorHandler::abortCompilation(msg);
  }

  _dex_file_env = make_shared<DexfileEnvironment>(dex_files);

  for (auto it : _modules) {
    auto id = it.first;
    auto module = it.second;
    auto codelib = module->createCodeLib();

    // initialize environment for codelib if present
    if (codelib != nullptr) {
      VLOG(artistd) << "ModuleManager: initializing codelib environment for module " << id;

      const MethodSignature signature = codelib->getCodeClass();
      const DexFile* codelib_dexfile = nullptr;
      for (auto && dexfile : dex_files) {
        // check whether `dexfile` defines the code lib class (lookup by signature)
        if (ArtUtils::FindClassDefIdxFromName(dexfile, signature)) {
          codelib_dexfile = dexfile;
          VLOG(artistd) << "ModuleManager: Found " << signature << " in dex file " << codelib_dexfile->GetLocation();
          break;
        }
      }
      if (codelib_dexfile == nullptr) {
        auto msg = "Could not find dexfile defining codelib class " + signature
                   + " (requires codelib). Deactivating module " + id + ".";
        VLOG(artist) << msg;
        module->setEnabled(false);
      } else {
        _dex_file_env->declareCodelib(codelib_dexfile);
        _environments[id] = make_shared<CodeLibEnvironment>(_dex_file_env, codelib_dexfile, codelib, jclass_loader);
      }
    }
  }

  VLOG(artistd) << "ModuleManager: initialized " << _environments.size() << " codelib environments (" << _modules.size() << " modules)";
  _init_flag = true;
}

  bool ModuleManager::initialized() const {
      return _init_flag;
  }
}  // namespace art
