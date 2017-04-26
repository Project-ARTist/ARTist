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

#include <string>
#include <vector>

#include "blacklist.h"

namespace art {

/** Blacklist certain packages or classes.
 *
 * Those methods won't get injected.
 *
 * If you want to blacklist a whole package and all of it's subpackages and
 * classes, be sure to include a trailing '.'
 *
 * You MUST add your root package of your Java Codelib,
 * otherwise artist crashes.
 *
 * You can also
 *
 */
const std::vector<std::string> BlackList::PACKAGE_BLACKLIST = {
    ".R$",              // => Should be left in place
    // Add your packages and classes below
    // >> Example (You can inject Android's support functions if you want to...)
    "android.support",
    // >> Included Demo CodeLib:
    "saarland.cispa.artist.codelib."
    // Add your packages and classes here above ^^
};

bool BlackList::IsBlacklisted(const std::string& method_name) {
  for (auto&& entry : PACKAGE_BLACKLIST) {
    if (method_name.find(entry) != std::string::npos) {
      return true;
    }
  }
  return false;
}

}  // namespace art
