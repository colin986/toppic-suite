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

#include "common/base/proteoform_type.hpp"

namespace toppic {

ProteoformTypePtr ProteoformType::COMPLETE = std::make_shared<ProteoformType>("COMPLETE", 0);

ProteoformTypePtr ProteoformType::PREFIX = std::make_shared<ProteoformType>("PREFIX", 1);

ProteoformTypePtr ProteoformType::SUFFIX = std::make_shared<ProteoformType>("SUFFIX", 2);

ProteoformTypePtr ProteoformType::INTERNAL = std::make_shared<ProteoformType>("INTERNAL", 3);

ProteoformType::ProteoformType(const std::string &name, int id): 
    name_(name), 
    id_(id) {}

ProteoformTypePtr ProteoformType::getProtTypePtrByName(std::string name) {
  if (name == "COMPLETE") {
    return ProteoformType::COMPLETE;
  }
  if (name == "PREFIX") {
    return ProteoformType::PREFIX;
  }
  if (name == "SUFFIX") {
    return ProteoformType::SUFFIX;
  }
  if (name == "INTERNAL") {
    return ProteoformType::SUFFIX;
  }
  return nullptr;
}

}  // namespace toppic
