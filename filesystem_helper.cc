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

#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <array>
#include <memory>

#include "base/logging.h"
#include "filesystem_helper.h"
#include "error_handler.h"

using std::make_shared;

namespace art {

const char FilesystemHelper::DEFAULT_APP[] = "/data/data/saarland.cispa.artist.artistgui/files/artist";
const char FilesystemHelper::DEFAULT_SYS[] = "TODO";

const char FilesystemHelper::MODULES_FOLDER[] = "modules";
const char FilesystemHelper::FILES_FOLDER[] = "files";
const char FilesystemHelper::TMP_FOLDER[] = "tmp";

const char FilesystemHelper::SETUP_ERROR[] = "Could not setup the directory at ";


FilesystemHelper::FilesystemHelper(const string base_path)
    : _base(base_path + "/"), _files(_base + FILES_FOLDER + "/"), _tmp(_base + TMP_FOLDER + "/") {
  if (!setupDir(_base)) {
    ErrorHandler::abortCompilation(SETUP_ERROR + _base);
  }
  if (!setupDir(_files)) {
    ErrorHandler::abortCompilation(SETUP_ERROR + _base);
  }
  if (!setupDir(_tmp)) {
    ErrorHandler::abortCompilation(SETUP_ERROR + _base);
  }
}

bool FilesystemHelper::isFile(const string path) {
  struct stat buf {};
  stat(path.c_str(), &buf);
  return S_ISREG(buf.st_mode);
}

bool FilesystemHelper::isDir(const string path) {
  struct stat buf {};
  stat(path.c_str(), &buf);
  return S_ISDIR(buf.st_mode);
}



bool FilesystemHelper::createDir(const string path) {
  return mkdir(path.c_str(), S_IRWXU) == 0;
}

bool FilesystemHelper::setupDir(const string path) {
  return isDir(path) || createDir(path);
}

const string FilesystemHelper::getFilesPath() const {
  return _files;
}

const string FilesystemHelper::getTmpPath() const {
  return _tmp;
}

shared_ptr<const FilesystemHelper> FilesystemHelper::createForModule(const string base, ModuleId id) {
  if (!setupDir(base)) {
    ErrorHandler::abortCompilation(SETUP_ERROR + base);
  }
  auto modules = base + "/" + MODULES_FOLDER + "/";
  if (!setupDir(modules)) {
    ErrorHandler::abortCompilation(SETUP_ERROR + modules);
  }
  auto concrete_module = modules + id;
  return make_shared<FilesystemHelper>(concrete_module);
}

}  // namespace art
