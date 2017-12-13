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

#include <atomic>

#include "universal_artist.h"
#include "optimizing/artist/artist_log.h"
#include "optimizing/artist/injection/injection_visitor.h"
#include "optimizing/artist/env/java_env.h"
#include "optimizing/artist/env/codelib.h"
#include "class_linker.h"
#include "class_linker-inl.h"
#include "optimizing/artist/injection/visitor_keys.h"


#include "optimizing/artist/injection/primitives.h"
#include "optimizing/artist/injection/boolean.h"
#include "optimizing/artist/injection/integer.h"
#include "optimizing/artist/injection/float.h"

#include "optimizing/artist/verbose_printer.h"

using std::shared_ptr;
using std::endl;
using std::sort;

namespace art {

void HUniversalArtist::SetupModule() {
    // get module-provided injections
    this->injections = ProvideInjections();
    // fill our internal data structures
    SetupInjections();  // FIXME this is horibly slow AND redundant since it is identical in each instance
}

void HUniversalArtist::SetupInjections() {
    VLOG(artistd) << "SetupInjections()";

//  VLOG(artist) << "HUniversalArtist::SetupInjections() InjectionCount Total #" << this->injections.size();
  int32_t targetCounter = 0;
  for (auto const & injection : this->injections) {
      VLOG(artistd) << "HUniversalArtist::SetupInjections() Method:      " << injection.GetSignature();
      auto injection_targets = injection.GetInjectionTargets();

      VLOG(artistd) << "HUniversalArtist::SetupInjections() Local TargetCount  #" << injection_targets.size();
      // TODO Bug: Using Injection with multiple targets that have the same InjectionTarget
      //           Leads to duplicate injections
      for (auto && target : injection_targets) {
          switch ((int32_t)target.GetTargetType()) {
              case InjectionTarget::METHOD_CALL_BEFORE:
                  ++targetCounter;
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_INTERFACE, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_STATIC_OR_DIRECT, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_VIRTUAL, injection);
                  break;
              case InjectionTarget::METHOD_CALL_AFTER:
                  ++targetCounter;
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_INTERFACE, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_STATIC_OR_DIRECT, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_INVOKE_VIRTUAL, injection);
                  break;
              case InjectionTarget::METHOD_START:
                  ++targetCounter;
                  this->EmplaceTableEntry(VisitorKeys::H_RETURN, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_RETURN_VOID, injection);
                  break;
              case InjectionTarget::METHOD_END:
                  ++targetCounter;
                  this->EmplaceTableEntry(VisitorKeys::H_RETURN, injection);
                  this->EmplaceTableEntry(VisitorKeys::H_RETURN_VOID, injection);
                  break;
              case InjectionTarget::NO_INJECTION:
              default:
                  VLOG(artistd) << "Nothing to inject";
                  continue;
          }
      }
  }
  VLOG(artistd) << "HUniversalArtist::SetupInjections() InjectionCount Total #" << this->injections.size();
  VLOG(artistd) << "HUniversalArtist::SetupInjections() TargetCount Total    #" << targetCounter;
  VLOG(artistd) << "HUniversalArtist::SetupInjections() DONE";
  VLOG(artistd) << endl;
}


void HUniversalArtist::RunModule()  {
  VLOG(artistd) << "Run Module " << this->GetPassName();
  HInjectionVisitor injectionVisitor(shared_from_this(), graph_);
  injectionVisitor.VisitInsertionOrder();
  VLOG(artistd) << "Run Module DONE";
}

// injection-specifics

const vector<Injection>& HUniversalArtist::GetInjections() {
    VLOG(artistd) << "HUniversalArtist::GetInjections()";
    return this->injections;
}

const unordered_map<string, vector<Injection>>& HUniversalArtist::GetInjectionTable() {
    return this->injection_table;
}

const vector<Injection> HUniversalArtist::GetInjectionTableEntry(const string& callback_key) {
    VLOG(artistd) << "HUniversalArtist::GetInjectionTableEntry()";
    unordered_map<string, vector<Injection>>::const_iterator found_item =
            this->injection_table.find(callback_key);

    if (found_item == this->injection_table.end()) {
        vector<Injection> localInjections = vector<Injection>();

        VLOG(artistd) << "HUniversalArtist::GetInjectionTableEntry() DONE: " << localInjections.size();
        return localInjections;
    } else {
        auto localInjections = this->injection_table.at(callback_key);

        VLOG(artistd) << "HUniversalArtist::GetInjectionTableEntry() DONE: " << localInjections.size();
        return localInjections;
    }
}

bool HUniversalArtist::EmplaceTableEntry(const string& callback_key,
                                           const Injection& single_injection) {
    VLOG(artistd) << "HUniversalArtist::EmplaceTableEntry()";

    vector<Injection> new_injections = GetInjectionTableEntry(callback_key);
    VLOG(artistd) << "HUniversalArtist::EmplaceTableEntry() " << callback_key
                  << " Table Contained    # " << new_injections.size();
    new_injections.push_back(single_injection);
    VLOG(artistd) << "HUniversalArtist::EmplaceTableEntry() " << callback_key
                  << " Table Contains Now # " << new_injections.size();
    this->injection_table.erase(callback_key);
    this->injection_table.emplace(callback_key, new_injections);
    VLOG(artistd) << "HUniversalArtist::EmplaceTableEntry() DONE";
    return true;
}

}  // namespace art
