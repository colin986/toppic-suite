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

#include "stat/local/local_mng.hpp"

namespace toppic {

LocalMng::LocalMng(PrsmParaPtr prsm_para_ptr,
                   double local_threshold,
                   const std::string& residueModFileName,
                   double max_ptm_mass,
                   double min_ptm_mass,
                   const std::string &input_file_ext,
                   const std::string &output_file_ext):
  prsm_para_ptr_(prsm_para_ptr),
  input_file_ext_(input_file_ext),
  output_file_ext_(output_file_ext),
  residueModFileName_(residueModFileName),
  threshold_(local_threshold),
  theta_(0.994),
  beta_(0.8),
  min_mass_(prsm_para_ptr->getSpParaPtr()->getMinMass()),
  max_ptm_mass_(max_ptm_mass),
  min_ptm_mass_(min_ptm_mass),
  p1_(0.915258),
  p2_(21.1822) {}

}  // namespace toppic

