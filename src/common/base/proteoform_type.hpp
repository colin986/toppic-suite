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

#ifndef TOPPIC_COMMON_BASE_PROTEOFORM_TYPE_HPP_
#define TOPPIC_COMMON_BASE_PROTEOFORM_TYPE_HPP_

#include <string>
#include <memory>
#include <vector>

namespace toppic {

class ProteoformType;
typedef std::shared_ptr<ProteoformType> ProteoformTypePtr;

class ProteoformType {
 public:
  static ProteoformTypePtr COMPLETE;
  static ProteoformTypePtr PREFIX;
  static ProteoformTypePtr SUFFIX;
  static ProteoformTypePtr INTERNAL;

  ProteoformType(const std::string &name, int id);

  std::string getName() {return name_;}

  int getId() {return id_;}

  static ProteoformTypePtr getProtTypePtrByName(std::string name);

 private:
  std::string name_;
  int id_;
};

typedef std::vector<ProteoformTypePtr> ProteoformTypePtrVec;

}  // namespace toppic

#endif
