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

bool ModuleManager::registerModule(const string id, const Module* module) {
    CHECK(module != nullptr);

    if (modules.find(id) != modules.end()) {
        return false;
    }
    modules[id] = module;
    // setup codelib environment if codelib is present (not all modules have a codelib)
    const CodeLib* codelib = module->createCodeLib();
    if (codelib != nullptr) {
        environments[id] = new CodeLibEnvironment(module->createCodeLib());
    }
    return true;
}

const Module* ModuleManager::getModule(const string id) const {
    if (modules.find(id) == modules.end()) {
        return nullptr;
    }
    return modules.at(id);
}

CodeLibEnvironment* ModuleManager::getCodeLibEnvironment(const string id) {
    if (modules.find(id) == modules.end()) {
        // some modules do not have a codelib, hence no corresponding environment
        return nullptr;
    }
    return environments.at(id);
}

    vector<const Module *>* ModuleManager::getModules() const {
        vector<const Module*>* result = new vector<const Module*>();
        for (auto it = modules.begin(); it != modules.end(); ++it) {
            result->push_back(it->second);
        }
        return result;
    }

}  // namespace art
