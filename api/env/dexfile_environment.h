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

#ifndef ART_API_ENV_DEXFILE_ENVIRONMENT_H_
#define ART_API_ENV_DEXFILE_ENVIRONMENT_H_

#include <string>
#include <vector>
#include "dex_file.h"

using std::string;
using std::vector;


namespace art {
class DexfileEnvironment {
 private:
    vector<const DexFile*> _app_dex_files;
    vector<const DexFile*> _codelibs;
    vector<const DexFile*> _all;

 public:
  explicit DexfileEnvironment(vector<const DexFile*>& dex_files);
  ~DexfileEnvironment();

  void declareCodelib(const DexFile* codelib_dex_file);

  const vector<const DexFile*>& getAppDexFiles() const;
  const vector<const DexFile*>& getCodelibDexFiles() const;
  const vector<const DexFile*>& getAllDexFiles() const;

  bool isCodelib(const DexFile* dex_file)const;
};

}  // namespace art




#endif  // ART_API_ENV_DEXFILE_ENVIRONMENT_H_
