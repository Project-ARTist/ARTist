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

#include "java_env.h"

namespace art {

const char JavaEnvironment::BOOLEAN = 'Z';
const char JavaEnvironment::BYTE    = 'B';
const char JavaEnvironment::CHAR    = 'C';
const char JavaEnvironment::DOUBLE  = 'D';
const char JavaEnvironment::FLOAT   = 'F';
const char JavaEnvironment::INT     = 'I';
const char JavaEnvironment::LONG    = 'J';
const char JavaEnvironment::SHORT   = 'S';
const char JavaEnvironment::VOID    = 'V';

const std::string JavaEnvironment::PRIMITIVES = "ZBCDFIJSV";

const char JavaEnvironment::CLASS = 'L';
const char JavaEnvironment::CLASS_TERMINATOR = ';';

const char JavaEnvironment::ARRAY = '[';

const char JavaEnvironment::FUNCTION_PARAM_START = '(';
const char JavaEnvironment::FUNCTION_PARAM_END = ')';

const std::string JavaEnvironment::C_STRING = "Ljava/lang/String;";
const std::string JavaEnvironment::C_OBJECT = "Ljava/lang/Object;";

const std::string JavaEnvironment::TESTER ="Lde/infsec/guess/whos/CLASS;FUNCTION(ZLjava/lang/String;BBSSSLjava/lang/String;Ljava/lang/String;B[Ljava/lang/String;C[D[FI[JLjava/lang/String;SV)V";


}  // namespace art
