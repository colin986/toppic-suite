//Copyright (c) 2014 - 2025, The Trustees of Indiana University, Tulane University.
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

#ifndef TOPPIC_PRSM_PRSM_FDR_GROUPS_HPP_
#define TOPPIC_PRSM_PRSM_FDR_GROUPS_HPP_

#include <string>

namespace toppic {

namespace prsm_fdr_groups {

void process(const std::string &spec_file_name,
             const std::string &input_file_ext,
             const std::string &output_file_ext,
             std::string keep_decoy_results);

}

}  // namespace toppic

#endif 
