//Copyright (c) 2014 - 2020, The Trustees of Indiana University.
//
//Licensed under the Apache License, Version 2.0 (the "License");
//you may not use this file except in compliance with the License.
//You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing, software
//distributed under the License is distributed on an "AS IS" BASIS,
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//See the License for the specific language governing permissions and
//limitations under the License.

#ifndef TOPPIC_MS_FEATURE_FEATURE_MERGE_HPP_
#define TOPPIC_MS_FEATURE_FEATURE_MERGE_HPP_

#include <memory>
#include <vector>
#include <string>

namespace toppic {

namespace feature_merge {

void process(const std::vector<std::string> &spec_file_names,
             const std::string &output_file_name,
             std::string &para_ptr);

}

} /* namespace toppic */

#endif
