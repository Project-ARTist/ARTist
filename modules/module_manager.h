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

#include "module.h"
#include "optimizing/artist/env/codelib_environment.h"

using std::map;
using std::vector;

namespace art {

// TODO thread-safety: synchronization/locking
// TODO find the right place for creating and storing the environment
class ModuleManager {
// singleton logic
 public:
    static ModuleManager* getInstance() {
        // Automated creation and destruction through `static` modifier.
        static ModuleManager instance;
        return &instance;
    }

    explicit ModuleManager(ModuleManager const&) = delete;
    void operator=(ModuleManager const&)  = delete;

 private:
    ModuleManager() {}
    ~ModuleManager() {
        // unlink and delete modules and environments
        for (auto pair : modules) {
            modules.erase(pair.first);
            delete pair.second;
        }
        for (auto pair : modules) {
            environments.erase(pair.first);
            delete pair.second;
        }
    }

// regular logic
 public:
    bool registerModule(const string id, const Module* module);

    const Module* getModule(const string id) const;
    CodeLibEnvironment* getCodeLibEnvironment(const string id);

    vector<const Module*>* getModules() const;

 private:
    map<const string, const Module*> modules;
    map<const string, CodeLibEnvironment*> environments;
};

}  // namespace art


#endif  // ART_MODULES_MODULE_MANAGER_H_
