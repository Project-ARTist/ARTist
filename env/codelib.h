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

#ifndef ART_ENV_CODELIB_H_
#define ART_ENV_CODELIB_H_

#include <string>
#include <unordered_set>

namespace art {

class CodeLib {
 public:
  static const std::unordered_set<std::string>& GetMethods() {
    return METHODS;
  };

 public:
  // CODE_GENERATOR ////////////////////////////////////////////////////////////////////////////////////////////////////  // METHODS //////////////////////////////////
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__GETCALLINGMETHODNAME____L;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__TRACELOG____V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGBOOLEAN__Z__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGCHAR__C__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGER__I__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGER__II__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGERTRACE__ZI__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGLONG__J__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGFLOAT__F__V;
  static const std::string _M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGDOUBLE__D__V;
  // FIELDS ///////////////////////////////////
  static const std::string _F_CODECLASS_INSTANCE;
  // CLASSES //////////////////////////////////
  static const std::string _C_JAVA_LANG_STRING;
  static const std::string _C_JAVA_LANG_OBJECT;
  static const std::string _C_CODECLASS;
// CODE_GENERATOR END //////////////////////////////////////////////////////////////////////////////////////////////////

 private:
  static const std::unordered_set<std::string> METHODS;

 private:
//  // Forbid Class Construction
//  CodeLib() {}
//  explicit CodeLib(CodeLib const&);  // Don't Implement
//  void operator=(CodeLib const&);    // Don't implement

 public:
  // Forbid Class Construction
  explicit CodeLib(CodeLib const&) = delete;
  void operator=(CodeLib const&) = delete;
  // Forbid Class Construction END
};  // class CodeLib

}  // namespace art

#endif  // ART_ENV_CODELIB_H_
