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

#include "optimizing/artist/env/codelib.h"
#include "optimizing/artist/artist.h"

#ifndef ART_MODULES_MODULE_H_
#define ART_MODULES_MODULE_H_

namespace art {

/**
 * Encapsulates an ARTist module that consists of an ARTist "optimization pass" and, optionally, a corresponding codelib.
 */
class Module {
 public:
  virtual ~Module() = default;

  /**
   * Create an "optimization pass" that is executed as a part of Optimizing's optimization framework.
   *
   * @param graph the current method graph
   * @param dex_compilation_unit
   * @return ARTist pass
   */
  virtual shared_ptr<HArtist> createPass(HGraph* graph, const DexCompilationUnit& dex_compilation_unit) const = 0;

  /**
   * Create an instance of the accompanying codelib, if any.
   *
   * @return codelib or nullptr
   */
  virtual shared_ptr<const CodeLib> createCodeLib() const = 0;

  /**
   * Set the enabled state of the ARTist module.
   *
   * @param enabled new enabled state
   */
  void setEnabled(bool enabled);

  /**
   * Return whether the module is currently enabled
   *
   * @return enabled state
   */
  bool isEnabled() const;

 private:
  bool _enabled = true;
};

}  // namespace art

#endif  // ART_MODULES_MODULE_H_
