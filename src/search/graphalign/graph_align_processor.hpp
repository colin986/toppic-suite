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


#ifndef TOPPIC_SEARCH_GRAPH_ALIGN_GRAPH_ALIGN_PROCESSOR_HPP_
#define TOPPIC_SEARCH_GRAPH_ALIGN_GRAPH_ALIGN_PROCESSOR_HPP_

#include "search/graph/graph.hpp"
#include "search/graphalign/graph_align_mng.hpp"

namespace toppic {

class GraphAlignProcessor {
 public:
  explicit GraphAlignProcessor(GraphAlignMngPtr mng_ptr): mng_ptr_(mng_ptr) {}
  void process();
 private:
  GraphAlignMngPtr mng_ptr_;
};

typedef std::shared_ptr<GraphAlignProcessor> GraphAlignProcessorPtr;

}  // namespace toppic

#endif

