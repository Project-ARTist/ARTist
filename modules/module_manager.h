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
#include <memory>

#include "module.h"
#include "optimizing/artist/env/codelib_environment.h"
#include "optimizing/artist/artist_typedefs.h"


using std::map;
using std::vector;
using std::atomic_bool;
using std::shared_ptr;
using std::make_shared;

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
    static ModuleManager& getInstance();

    explicit ModuleManager(ModuleManager const&) = delete;
    explicit ModuleManager(ModuleManager &&) noexcept = delete;
    void operator=(ModuleManager const&)  = delete;
    void operator=(ModuleManager &&) noexcept = delete;
    ~ModuleManager() = default;

 private:
    ModuleManager() : _init_flag(false), _modules {}, _dex_file_env(), _environments {} {}

// regular logic
 public:
    bool registerModule(ModuleId id, shared_ptr<Module> module);

    shared_ptr<Module> getModule(ModuleId id) const;
    shared_ptr<const DexfileEnvironment> getDexFileEnvironment() const;
    shared_ptr<CodeLibEnvironment> getCodelibEnvironment(ModuleId id) const;

    const map<ModuleId, shared_ptr<Module>> getModules() const;

    void initializeModules(vector<const DexFile*> dex_files, jobject jclass_loader);
    bool initialized() const;

 private:
    atomic_bool _init_flag;
    map<ModuleId, shared_ptr<Module>> _modules;
    shared_ptr<DexfileEnvironment> _dex_file_env;
    map<ModuleId, shared_ptr<CodeLibEnvironment>> _environments;
};

}  // namespace art


#endif  // ART_MODULES_MODULE_MANAGER_H_
