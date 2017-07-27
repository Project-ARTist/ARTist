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
 *
 */

#include <algorithm>

#include "dexfile_environment.h"

using std::find;

namespace art {

const vector<const DexFile*>& DexfileEnvironment::getAppDexFiles() const {
    return _app_dex_files;
}

const vector<const DexFile*>& DexfileEnvironment::getCodelibDexFiles() const {
    return *_codelibs;
}

const vector<const DexFile*>& DexfileEnvironment::getAllDexFiles() const {
    return _all;
}

bool DexfileEnvironment::isCodelib(const DexFile* dex_file) const {
    if (_codelibs->empty()) {
        return false;
    }
    bool result = find(_codelibs->begin(), _codelibs->end(), dex_file) != _codelibs->end();
    return result;
}

    DexfileEnvironment::~DexfileEnvironment() {
        delete _codelibs;
    }

}  // namespace art
