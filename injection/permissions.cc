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

#include "permissions.h"

namespace art {

const std::string Permissions::ACCESS_COARSE_LOCATION = "android.permission.ACCESS_COARSE_LOCATION";
const std::string Permissions::ACCESS_FINE_LOCATION = "android.permission.ACCESS_FINE_LOCATION";
const std::string Permissions::ACCESS_WIFI_STATE = "android.permission.ACCESS_WIFI_STATE";
const std::string Permissions::BLUETOOTH = "android.permission.BLUETOOTH";
const std::string Permissions::BLUETOOTH_ADMIN = "android.permission.BLUETOOTH_ADMIN";
const std::string Permissions::READ_PHONE_STATE = "HInvoke";


}  // namespace art
