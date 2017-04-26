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

#ifndef ART_INJECTION_PERMISSIONS_H_
#define ART_INJECTION_PERMISSIONS_H_

#include <string>

namespace art {

enum PermissionId { NO_PERMISSION = -1
                  , ACCESS_COARSE_LOCATION = 0
                  , ACCESS_FINE_LOCATION = 1
                  , ACCESS_WIFI_STATE = 2
                  , BLUETOOTH = 3
                  , BLUETOOTH_ADMIN = 4
                  , READ_PHONE_STATE = 5
};

class Permissions {
 public:
  static const std::string ACCESS_COARSE_LOCATION;
  static const std::string ACCESS_FINE_LOCATION;
  static const std::string ACCESS_WIFI_STATE;
  static const std::string BLUETOOTH;
  static const std::string BLUETOOTH_ADMIN;
  static const std::string READ_PHONE_STATE;
};

}  // namespace art

#endif  // ART_INJECTION_PERMISSIONS_H_
