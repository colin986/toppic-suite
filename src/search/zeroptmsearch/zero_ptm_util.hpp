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

#ifndef TOPPIC_SEARCH_ZERO_PTM_SEARCH_ZERO_PTM_UTIL_HPP_
#define TOPPIC_SEARCH_ZERO_PTM_SEARCH_ZERO_PTM_UTIL_HPP_

#include "para/sp_para.hpp"
#include "ms/spec/deconv_peak.hpp"
#include "prsm/prsm.hpp"
#include "search/zeroptmsearch/zero_ptm_fast_match.hpp"

namespace toppic {

namespace zero_ptm_util {

PrsmPtr getPrsmPtr(const DeconvMsPtrVec &deconv_ms_ptr_vec, 
                   ZpFastMatchPtr fast_match_ptr,
                   SpParaPtr sp_para_ptr);

PrsmPtr getPrsmPtr(const DeconvMsPtrVec &deconv_ms_ptr_vec, 
                   ProteoformPtr proteoform_ptr,
                   SpParaPtr sp_para_ptr);

}

}

#endif
