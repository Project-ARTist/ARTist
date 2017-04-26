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

#ifndef ART_ENV_JAVA_ENV_H_
#define ART_ENV_JAVA_ENV_H_

#include <string>

namespace art {


/** Definitions according to
 *  "The Java class File Format" https://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 */
class JavaEnvironment {
 public:
  static const char BOOLEAN;
  static const char BYTE;
  static const char CHAR;
  static const char DOUBLE;
  static const char FLOAT;
  static const char INT;
  static const char LONG;
  static const char SHORT;
  static const char VOID;

  static const std::string PRIMITIVES;

  static const char CLASS;
  static const char CLASS_TERMINATOR;
  static const char ARRAY;
  static const char FUNCTION_PARAM_START;
  static const char FUNCTION_PARAM_END;

  static const std::string TESTER;

  // Classes are prefixed wit C_
  static const std::string C_STRING;
  static const std::string C_OBJECT;
};

}  // namespace art



#endif  // ART_ENV_JAVA_ENV_H_
