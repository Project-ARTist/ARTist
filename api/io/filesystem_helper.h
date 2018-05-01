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

#ifndef ART_API_IO_FILESYSTEM_HELPER_H_
#define ART_API_IO_FILESYSTEM_HELPER_H_

#include <string>
#include "optimizing/artist/api/env/artist_typedefs.h"

using std::string;
using std::shared_ptr;

namespace art {


/**
 * A helper class for ARTist and its modules to provide convenient access to readable and writable directories.
 */
class FilesystemHelper {
 public:
  static const char DEFAULT_APP[];
  static const char DEFAULT_SYS[];

  explicit FilesystemHelper(const string base_path);
  const string getFilesPath() const;
  const string getTmpPath() const;

  static shared_ptr<const FilesystemHelper> createForModule(const string base, ModuleId id);


 private:
  static const char MODULES_FOLDER[];
  static const char FILES_FOLDER[];
  static const char TMP_FOLDER[];

  static const char SETUP_ERROR[];

  /**
   * Check whether the given path points to an existing file.
   * @param path the path to check
   * @return true if path is a file, false if non-existent or not a file.
   */
  static bool isFile(const string path);

  /**
   * Check whether the given path points to an existing directory.
   * @param path the path to check
   * @return true if path is a directory, false if non-existent or not a directory.
   */
  static bool isDir(const string path);

  /**
   * Creates a directory that is defined by the last segment of the given path.
   * @param path the new directory
   * @return true if it was successfully created, false otherwise (also if the dir existed before)
   */
  static bool createDir(const string path);

  /**
   * Ensures that `path` is a valid directory by checking whether it already is one and if not creating it.
   * Use this if you do not care whether the directory already exists or not because, e.g., you just want to write files.
   * @param path the path to the target directory
   * @return whether `path` points to a valid directory now
   */
  static bool setupDir(const string path);

  const string _base;
  const string _files;
  const string _tmp;
};

}  // namespace art


#endif  // ART_API_IO_FILESYSTEM_HELPER_H_
