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

#include "trace_module.h"
#include "trace_artist.h"
#include "trace_codelib.h"
#include "optimizing/artist/filtering/method_name_filters.h"

using std::make_shared;
using std::unique_ptr;

namespace art {

    shared_ptr<HArtist> TraceModule::createPass(const MethodInfo& method_info) const {
//        return new /*(graph->GetArena())*/ HTraceArtist(method_info);
        return make_shared<HTraceArtist>(method_info);
    }

    shared_ptr<const CodeLib> TraceModule::createCodeLib() const {
        return make_shared<TraceCodeLib>();
    }

    // skip android support lib ui methods since they bloat up the log
    unique_ptr<Filter> TraceModule::getMethodFilter() const {
      const vector<const string> blackListDefinition = {
          "android.support.",
          "com.package.app.specific.Class"
      };
      auto blackList = unique_ptr<Filter>(new BlacklistFilter(blackListDefinition));

      const vector<const string> whiteListDefinition = {
          "com.package.app."
      };
      auto whiteList = unique_ptr<Filter>(new WhitelistFilter(whiteListDefinition));

      auto dualFilter = unique_ptr<Filter>(new DualFilter(whiteList, blackList));

      return dualFilter;
    }

}  // namespace art
