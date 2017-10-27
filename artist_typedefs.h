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
 *
 */

#ifndef ART_ARTIST_TYPEDEFS_H_
#define ART_ARTIST_TYPEDEFS_H_

#include <string>


using std::string;

namespace art {
// modules
typedef const string ModuleId;

// indices
typedef uint16_t ClassDefIdx;
typedef uint16_t TypeIdx;
typedef uint32_t FieldIdx;
typedef uint32_t MethodIdx;
typedef size_t MethodVtableIdx;

// method signature
typedef string MethodSignature;
typedef string ClassSignature;
}  // namespace art

#endif  // ART_ARTIST_TYPEDEFS_H_
