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

DexfileEnvironment::DexfileEnvironment(vector<const DexFile *> &dex_files) : _app_dex_files(dex_files), _codelibs(0),
                                                                             _all(dex_files) {}

void DexfileEnvironment::declareCodelib(const DexFile* codelib_dex_file) {
  CHECK(codelib_dex_file != nullptr);
  VLOG(artistd) << "declaring dex file as codelib: " << codelib_dex_file->GetLocation();

  // preconditions: all dex files are known beforehand and we only, step by step, move codelib dex files over from
  // the app dex files list
  CHECK(find(_all.begin(), _all.end(), codelib_dex_file) != _all.end());

  auto app_iter = find(_app_dex_files.begin(), _app_dex_files.end(), codelib_dex_file);
  CHECK(app_iter != _all.end());
  _app_dex_files.erase(app_iter);

  CHECK(find(_codelibs.begin(), _codelibs.end(), codelib_dex_file) == _codelibs.end());
  _codelibs.push_back(codelib_dex_file);
}


const vector<const DexFile*>& DexfileEnvironment::getAppDexFiles() const {
  return _app_dex_files;
}

const vector<const DexFile*>& DexfileEnvironment::getCodelibDexFiles() const {
  return _codelibs;
}

const vector<const DexFile*>& DexfileEnvironment::getAllDexFiles() const {
  return _all;
}

bool DexfileEnvironment::isCodelib(const DexFile* dex_file) const {
  if (_codelibs.empty()) {
      return false;
  }
  bool result = find(_codelibs.begin(), _codelibs.end(), dex_file) != _codelibs.end();
  return result;
}

DexfileEnvironment::~DexfileEnvironment() {}

}  // namespace art
