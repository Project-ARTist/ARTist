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

#ifndef ART_MODULES_GENERIC_INJECTIONS_H_
#define ART_MODULES_GENERIC_INJECTIONS_H_

#include <memory>
#include "optimizing/artist/injection/short.h"
#include "optimizing/artist/injection/boolean.h"
#include "optimizing/artist/injection/byte.h"
#include "optimizing/artist/injection/char.h"
#include "optimizing/artist/injection/double.h"
#include "optimizing/artist/injection/float.h"
#include "optimizing/artist/injection/integer.h"
#include "optimizing/artist/injection/long.h"
#include "optimizing/artist/env/codelib.h"
#include "optimizing/artist/injection/injection.h"

namespace art {

class HInjections {
 public:
  static std::vector<Injection> buildInjections() {
      std::vector<Injection> injections;
      // TODO Bug: Using Injection with multiple targets that have the same InjectionTarget
      //           Leads to duplicate injections
      //
      // **Define injections here**
      //
      // => Generate your own codelib.h/.cc and check available methods (signatures) for injection.
      //
      // <Injections>
      // Trace Logging in all onCreate() methods.
      // Parameter and package-/class-names get ignored
      {
          Injection trace_logging_specific(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__TRACELOG____V,
              std::vector<std::shared_ptr<Parameter>>(),  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
      }
      {
          auto log_bool_params = std::vector<shared_ptr<Parameter>>();
          auto bool_param = std::make_shared<Integer>(true);
          log_bool_params.push_back(bool_param);
          Injection log_bool(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGBOOLEAN__Z__V,
              log_bool_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_bool);
      }
      {
          auto log_byte_params = std::vector<shared_ptr<Parameter>>();
          auto byte_param = std::make_shared<Byte>(8);
          log_byte_params.push_back(byte_param);
          Injection log_byte(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGER__I__V,
              log_byte_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_byte);
      }
      {
          auto log_short_params = std::vector<shared_ptr<Parameter>>();
          auto short_param = std::make_shared<Short>(16);
          log_short_params.push_back(short_param);
          Injection log_short(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGER__I__V,
              log_short_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_short);
      }
      {
          auto log_char_params = std::vector<shared_ptr<Parameter>>();
          auto char_param = std::make_shared<Char>('z');
          log_char_params.push_back(char_param);
          Injection log_char(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGCHAR__C__V,
              log_char_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_char);
      }
      {
          auto log_integer_params = std::vector<shared_ptr<Parameter>>();
          auto integer_param = std::make_shared<Integer>(12345);
          log_integer_params.push_back(integer_param);
          Injection log_integer(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGINTEGER__I__V,
              log_integer_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_integer);
      }
      {
          auto log_long_params = std::vector<shared_ptr<Parameter>>();
          auto long_param = std::make_shared<Long>(12345678);
          log_long_params.push_back(long_param);
          Injection log_long(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGLONG__J__V,
              log_long_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_long);
      }
      {
          auto log_float_params = std::vector<shared_ptr<Parameter>>();
          auto float_param = std::make_shared<Float>(1.23456);
          log_float_params.push_back(float_param);
          Injection log_float(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGFLOAT__F__V,
              log_float_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_float);
      }
      {
          auto log_double_params = std::vector<shared_ptr<Parameter>>();
          auto double_param = std::make_shared<Double>(12345.67890);
          log_double_params.push_back(double_param);
          Injection log_double(
              CodeLib::_M_SAARLAND_CISPA_ARTIST_CODELIB_CODELIB__LOGDOUBLE__D__V,
              log_double_params,  // No params
              Target(".onCreate(", InjectionTarget::METHOD_START));
          injections.push_back(log_double);
      }
      // </Injections>
      return injections;
  }
};

}  // namespace art

#endif  // ART_MODULES_GENERIC_INJECTIONS_H_
