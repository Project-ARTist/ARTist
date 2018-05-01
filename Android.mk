#
# The ARTist Project (https://artist.cispa.saarland)
#
# Copyright (C) 2017 CISPA (https://cispa.saarland), Saarland University
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# @author "Oliver Schranz <oliver.schranz@cispa.saarland>"
# @author "Sebastian Weisgerber <weisgerber@cispa.saarland>"
#

# @TODO:
# - Create own .so for artist

# Append Artist sourcefiles to LIBART_COMPILER_SRC_FILES variable
LIBART_COMPILER_SRC_FILES += \
    optimizing/artist/api/modules/artist.cc \
    optimizing/artist/api/io/artist_log.cc \
    optimizing/artist/api/utils/artist_utils.cc \
    optimizing/artist/api/io/error_handler.cc \
    optimizing/artist/api/filtering/method_name_filters.cc \
    optimizing/artist/api/env/java_env.cc \
    optimizing/artist/api/env/codelib_environment.cc \
    optimizing/artist/api/env/codelib_symbols.cc \
    optimizing/artist/api/env/dexfile_environment.cc \
    optimizing/artist/api/injection/injection.cc \
    optimizing/artist/internal/injection/injection_visitor.cc \
    optimizing/artist/api/injection/parameter.cc \
    optimizing/artist/api/injection/target.cc \
    optimizing/artist/internal/injection/visitor_keys.cc \
    optimizing/artist/api/injection/injection_artist.cc \
    optimizing/artist/api/modules/method_info.cc \
    optimizing/artist/api/modules/method_info_factory.cc \
    optimizing/artist/internal/utils/param_finder.cc \
    optimizing/artist/api/modules/module.cc \
    optimizing/artist/api/modules/module_manager.cc \
    optimizing/artist/api/io/verbose_printer.cc \
    optimizing/artist/api/io/filesystem_helper.cc \
    optimizing/artist/internal/modules/logtimization/logtimization_artist.cc \
    optimizing/artist/internal/modules/logtimization/logtimization_module.cc \
    optimizing/artist/internal/modules/trace/trace_artist.cc \
    optimizing/artist/internal/modules/trace/trace_module.cc \
    optimizing/artist/internal/modules/trace/trace_codelib.cc \

