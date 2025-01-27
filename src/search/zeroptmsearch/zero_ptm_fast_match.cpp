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

#include "search/zeroptmsearch/zero_ptm_fast_match.hpp"

namespace toppic {

ZeroPtmFastMatch::ZeroPtmFastMatch(ProteoformPtr proteo_ptr, 
                                   double score, 
                                   int begin, int end):
      proteo_ptr_(proteo_ptr),
      score_(score),
      begin_(begin),
      end_(end) {}

bool ZeroPtmFastMatch::cmpScoreDecProtNameInc(const ZpFastMatchPtr &a, 
                                              const ZpFastMatchPtr &b) {
  if (a->getScore() > b->getScore()) {
    return true;
  }
  else if (a->getScore() < b->getScore()) {
    return false;
  }
  else if (a->getProteoformPtr()->getSeqName() <
           b->getProteoformPtr()->getSeqName()) {
    return true;
  }
  else {
    return false;
  }
}

}  // namespace toppic
