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

#ifndef ART_MODULES_MODULE_MANAGER_H_
#define ART_MODULES_MODULE_MANAGER_H_

#include <atomic>

#include "module.h"
#include "optimizing/artist/env/codelib_environment.h"
#include "optimizing/artist/artist_typedefs.h"


using std::map;
using std::vector;
using std::atomic_bool;

namespace art {

// TODO thread-safety: think about where or if synchronization/locking is needed?
// TODO find the right place for creating and storing the environment
/**
 * Manages ARTist modules and provides access to their codelib environments.
 * After all the modules have been registered, ``initializeModules`` needs to be called.
 *
 * The singleton pattern is used because we need easy and shared access to the modules and corresponding environments
 * in the threads that compile all app methods.
 */
class ModuleManager {
// singleton logic
 public:
    static ModuleManager* getInstance();

    explicit ModuleManager(ModuleManager const&) = delete;
    void operator=(ModuleManager const&)  = delete;

 private:
    ModuleManager() : init_flag(false) {}
    ~ModuleManager();

    bool definesClass(const DexFile* dexfile, const MethodSignature searched_signature) const;

// regular logic
 public:
    bool registerModule(ModuleId id, const Module* module);

    const Module* getModule(ModuleId id) const;
    const DexfileEnvironment* getDexFileEnvironment() const;
    CodeLibEnvironment* getCodelibEnvironment(ModuleId id) const;

    const map<ModuleId, const Module*> getModules() const;

    void initializeModules(DexfileEnvironment* dexfile_env, jobject jclass_loader);
    bool initialized() const;

 private:
    atomic_bool init_flag;
    map<ModuleId, const Module*> _modules;
    DexfileEnvironment* _dexfile_env;
    map<ModuleId, CodeLibEnvironment*> _environments;
};

}  // namespace art


#endif  // ART_MODULES_MODULE_MANAGER_H_
