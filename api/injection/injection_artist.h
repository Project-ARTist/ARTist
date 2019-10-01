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

#ifndef ART_API_INJECTION_INJECTION_ARTIST_H_
#define ART_API_INJECTION_INJECTION_ARTIST_H_

#include "optimizing/artist/api/modules/artist.h"

using std::unordered_map;
using std::enable_shared_from_this;

namespace art {

/**
 * Special kind of ARTist passes that allows for easy injection of method calls by simply declaring an injection policy.
 * Universal artist passes only need to overwrite a single method to declare what method calls should be injected
 * where and when.
 * This is well suited to build, e.g., inline reference monitors or logging modules where code is only added.
 */
class HInjectionArtist : public HArtist {
 public:
  explicit HInjectionArtist(const ArtistMethodInfo& method_info,
#ifdef BUILD_MARSHMALLOW
    bool is_in_ssa_form = true,
#endif
    const char* pass_name = "InjectionArtist"
    , OptimizingCompilerStats* stats = nullptr)
    : HArtist(method_info
#ifdef BUILD_MARSHMALLOW
        , is_in_ssa_form
#endif
        , pass_name
        , stats)
      , _injections {}
      , _injection_table {} {}

  // artist module interface
  void SetupPass() OVERRIDE;
  void RunPass() OVERRIDE;

  const vector<shared_ptr<const Injection>>& GetInjections();
  const unordered_map<string, vector<shared_ptr<const Injection>>>& GetInjectionTable();
  const vector<shared_ptr<const Injection>> GetInjectionTableEntry(const string& callback_key);
  bool EmplaceTableEntry(const string& callback_key, shared_ptr<const Injection> single_injection);

 protected:
  /**
   * Provides a list of configurations that governs the process of injecting method calls.
   * This method defines what method calls the concrete module is injection and where.
   *
   * @return list of injection policies
   */
  virtual vector<shared_ptr<const Injection>> ProvideInjections() const = 0;

 private:
  void SetupInjections();

  vector<shared_ptr<const Injection>> _injections;

  // Use @see VisitorKeys
  unordered_map<string, vector<shared_ptr<const Injection>>> _injection_table;
};

}  // namespace art

#endif  // ART_API_INJECTION_INJECTION_ARTIST_H_
