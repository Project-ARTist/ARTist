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

#include "trace_codelib.h"

namespace art {

const std::string TraceCodeLib::TRACELOG =
            "Lsaarland/cispa/artist/codelib/CodeLib;traceLog()V";

unordered_set<string>& TraceCodeLib::getMethods() const {
    static unordered_set<string> methods({TRACELOG});
    return methods;
}
string& TraceCodeLib::getInstanceField() const {
    static string instanceField = "Lsaarland/cispa/artist/codelib/CodeLib;INSTANCE";
    return instanceField;
}
string& TraceCodeLib::getCodeClass() const {
    static string codeClass = "Lsaarland/cispa/artist/codelib/CodeLib;";
    return codeClass;
}
}  // namespace art
