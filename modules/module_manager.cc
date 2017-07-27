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

namespace art {

ModuleManager* ModuleManager::getInstance() {
    // Automated creation and destruction through `static` modifier.
    static ModuleManager instance;
    return &instance;
}

bool ModuleManager::registerModule(ModuleId id, const Module* module) {
    CHECK(module != nullptr);
    VLOG(artistd) << "ModuleManager: registering module " << id;

    if (_modules.find(id) != _modules.end()) {
        return false;
    }
    _modules[id] = module;
    return true;
}

const Module* ModuleManager::getModule(ModuleId id) const {
    if (_modules.find(id) == _modules.end()) {
        return nullptr;
    }
    return _modules.at(id);
}

const DexfileEnvironment* ModuleManager::getDexFileEnvironment() const {
    return _dexfile_env;
}

CodeLibEnvironment* ModuleManager::getCodelibEnvironment(ModuleId id) const {
    CHECK(initialized());
    if (_environments.find(id) == _environments.end()) {
        // some modules do not have a codelib, hence no corresponding environment
        VLOG(artistd) << "ModuleManager: could not find codelib env for module " + id;
        return nullptr;
    }
    VLOG(artistd) << "ModuleManager: returning codelib env for module " + id;
    return _environments.at(id);
}

const map<ModuleId, const Module*> ModuleManager::getModules() const {
    VLOG(artistd) << "ModuleManager: obtaining the modules map with " << _modules.size() << " entries.";
    return _modules;
}

void ModuleManager::initializeModules(DexfileEnvironment *dexfile_env, jobject jclass_loader) {
    VLOG(artistd) << "ModuleManager: initializing modules ";
    if (init_flag.exchange(true)) {  // atomic check and set
        string msg = "ModuleManager: Attempting to initialize modules more than once.";
        ArtUtils::abort(msg);
    }

    _dexfile_env = dexfile_env;

    for (auto it : _modules) {
        auto id = it.first;
        auto module = it.second;
        auto codelib = module->createCodeLib();

        // initialize environment for codelib if present
        if (codelib != nullptr) {
            VLOG(artistd) << "ModuleManager: initializing codelib environment for module " << id;

            const MethodSignature signature = codelib->getCodeClass();
            const DexFile* codelib_dexfile = nullptr;
            for (auto && dexfile : dexfile_env->getCodelibDexFiles()) {
                // check whether `dexfile` defines the code lib class (lookup by signature)
                if (definesClass(dexfile, signature)) {
                    codelib_dexfile = dexfile;
                    VLOG(artistd) << "ModuleManager: Found " << signature << " in dex file " << codelib_dexfile->GetLocation();
                    break;
                }
            }
            if (codelib_dexfile == nullptr) {
                auto msg = "Could not find dexfile defining codelib class " + signature;
                ArtUtils::abort(msg);
            }
            _environments[id] = new CodeLibEnvironment(dexfile_env, codelib_dexfile, codelib, jclass_loader);
        }
    }
    VLOG(artistd) << "ModuleManager: initialized " << _environments.size() << " codelib environments (" << _modules.size() << " modules)";
    init_flag = true;
}

    bool ModuleManager::initialized() const {
        return init_flag;
    }

    /**
     * Check whether the given dexfile defines the given class
     *
     * @param dexfile the dex file to be searched
     * @param searched_signature the signature of the searched class
     * @return whether the dex file defines the class
     */
    bool ModuleManager::definesClass(const DexFile *dexfile, const MethodSignature searched_signature) const {
        for (uint16_t idx = 0; idx < dexfile->NumClassDefs(); idx++) {
            auto class_def = &dexfile->GetClassDef(idx);
            ClassDefIdx class_idx = class_def->class_idx_;
            auto candidate_signature = dexfile->StringByTypeIdx(class_idx);
            VLOG(artistd) << "ModuleManager: comparing candidate " << candidate_signature << " to searched " << searched_signature;
            if (searched_signature.find(candidate_signature) != string::npos) {
                return true;
            }
        }
        return false;
    }

    ModuleManager::~ModuleManager() {
        // FIXME this crashes the compiler
        // unlink and delete modules and environments
//        for (auto pair : _modules) {
//            _modules.erase(pair.first);
//            delete pair.second;
//        }
//        VLOG(artist) << "DEBUG: ModuleManager: deleting " << _environments.size() << " environments";
//        for (auto pair : _environments) {
//            _environments.erase(pair.first);
//            delete pair.second;
//            VLOG(artist) << "successfully deleted environment";
//        }
//        VLOG(artist) << "DEBUG: ModuleManager: AFTER deleting environments";
    }
}  // namespace art
