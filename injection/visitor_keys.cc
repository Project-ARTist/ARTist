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
 * @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
 *
 */

#include "visitor_keys.h"

namespace art {

const std::string VisitorKeys::H_INVOKE = "HInvoke";
const std::string VisitorKeys::H_INVOKE_INTERFACE = "HInvokeInterface";
const std::string VisitorKeys::H_INVOKE_STATIC_OR_DIRECT = "HInvokeStaticOrDirect";
const std::string VisitorKeys::H_INVOKE_VIRTUAL = "HInvokeVirtual";

const std::string VisitorKeys::H_RETURN = "HReturn";
const std::string VisitorKeys::H_RETURN_VOID = "HReturnVoid";

}  // namespace art
