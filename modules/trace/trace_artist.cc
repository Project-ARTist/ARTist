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

#include "trace_artist.h"
#include "trace_codelib.h"
#include "optimizing/artist/artist_log.h"
#include "optimizing/artist/injection/injection_visitor.h"
#include "optimizing/artist/env/java_env.h"
#include "class_linker-inl.h"

#include "optimizing/artist/injection/primitives.h"
#include "optimizing/artist/injection/boolean.h"
#include "optimizing/artist/injection/integer.h"
#include "optimizing/artist/injection/float.h"

#include "optimizing/artist/verbose_printer.h"

#include "optimizing/artist/injection/char.h"

using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::endl;
using std::sort;


namespace art {

vector<shared_ptr<const Injection>> HTraceArtist::ProvideInjections() const {
  VLOG(artistd) << "HTraceArtist::ProvideInjections()";

  vector<shared_ptr<const Parameter>> params;
  auto param = make_shared<const Char>();
  params.push_back(param);

  vector<shared_ptr<const Target>> targets;
  auto target = make_shared<const Target>(Target::GENERIC_TARGET, InjectionTarget::METHOD_END);
//  const Target target(".onCreate(", InjectionTarget::METHOD_START);
  targets.push_back(target);

  auto injection = make_shared<const Injection>(TraceCodeLib::TRACELOG, params, targets);

  vector<shared_ptr<const Injection>> results;
  results.push_back(injection);
  return results;
}

}  // namespace art
