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

#include "injection_artist.h"
#include "optimizing/artist/api/io/artist_log.h"
#include "optimizing/artist/internal/injection/injection_visitor.h"
#include "optimizing/artist/api/env/java_env.h"
#include "optimizing/artist/internal/injection/visitor_keys.h"


#include "primitives.h"
#include "optimizing/artist/api/injection/boolean.h"
#include "integer.h"
#include "optimizing/artist/api/injection/float.h"

#include "optimizing/artist/api/io/verbose_printer.h"

using std::shared_ptr;
using std::endl;
using std::sort;

namespace art {

void HInjectionArtist::SetupPass() {
    // get module-provided injections
    this->_injections = ProvideInjections();
    // fill our internal data structures
    SetupInjections();  // FIXME this is horibly slow AND redundant since it is identical in each instance
}

void HInjectionArtist::SetupInjections() {
    VLOG(artistd) << "SetupInjections()";

//  VLOG(artist) << "HInjectionArtist::SetupInjections() InjectionCount Total #" << this->injections.size();
  int32_t targetCounter = 0;
  for (auto const injection : this->_injections) {
      VLOG(artistd) << "HInjectionArtist::SetupInjections() Method:      " << injection->GetSignature();
      auto injection_targets = injection->GetInjectionTargets();

      VLOG(artistd) << "HInjectionArtist::SetupInjections() Local TargetCount  #" << injection_targets.size();
      // TODO Bug: Using Injection with multiple targets that have the same InjectionTarget
      //           Leads to duplicate injections
      for (auto && target : injection_targets) {
          switch ((int32_t)target->GetTargetType()) {
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
  VLOG(artistd) << "HInjectionArtist::SetupInjections() InjectionCount Total #" << this->_injections.size();
  VLOG(artistd) << "HInjectionArtist::SetupInjections() TargetCount Total    #" << targetCounter;
  VLOG(artistd) << "HInjectionArtist::SetupInjections() DONE";
  VLOG(artistd) << endl;
}


void HInjectionArtist::RunPass()  {
  VLOG(artistd) << "Run Pass " << this->GetPassName();
  HInjectionVisitor injectionVisitor(this, graph_);
  injectionVisitor.VisitInsertionOrder();
  VLOG(artistd) << "Run Pass DONE";
}

// injection-specifics

const vector<shared_ptr<const Injection>>& HInjectionArtist::GetInjections() {
    VLOG(artistd) << "HInjectionArtist::GetInjections()";
    return this->_injections;
}

const unordered_map<string, vector<shared_ptr<const Injection>>>& HInjectionArtist::GetInjectionTable() {
    return this->_injection_table;
}

const vector<shared_ptr<const Injection>> HInjectionArtist::GetInjectionTableEntry(const string& callback_key) {
  auto found_item = this->_injection_table.find(callback_key);
  vector<shared_ptr<const Injection>> local_injections;
  if (found_item == this->_injection_table.end()) {
    return local_injections;
  } else {
    local_injections = this->_injection_table.at(callback_key);
    return local_injections;
  }
}

bool HInjectionArtist::EmplaceTableEntry(const string& callback_key, shared_ptr<const Injection> single_injection) {
    VLOG(artistd) << "HInjectionArtist::EmplaceTableEntry()";

    auto new_injections = GetInjectionTableEntry(callback_key);
    VLOG(artistd) << "HInjectionArtist::EmplaceTableEntry() " << callback_key
                  << " Table Contained    # " << new_injections.size();
    new_injections.push_back(single_injection);
    VLOG(artistd) << "HInjectionArtist::EmplaceTableEntry() " << callback_key
                  << " Table Contains Now # " << new_injections.size();
    this->_injection_table.erase(callback_key);
    this->_injection_table.emplace(callback_key, new_injections);
    VLOG(artistd) << "HInjectionArtist::EmplaceTableEntry() DONE";
    return true;
}

}  // namespace art
