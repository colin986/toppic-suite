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

#ifndef TOPPIC_SEQ_ALTER_HPP_
#define TOPPIC_SEQ_ALTER_HPP_

#include "common/xml/xml_dom_element.hpp"
#include "common/base/mod.hpp"
#include "seq/alter_type.hpp"
#include "seq/local_anno.hpp"

namespace toppic {

class XmlDOMDocument;

// A mass shift is explained by a list of alterations (alter class).  
//
// Case 1: If the PTM in an alteration is identified but not localized, then the
// PTM information is stored in mod_ptr_. Since the modified amino acid is
// unknown, the mod instance will contain an empty amino acid and a modified
// version of the empty amino acid. 
//
// In addition, if the alteration mass is charaterized by MIScore, but not
// fulled charaterized, then the output of MIScore method is store in
// local_anno_ptr_. 
//
// Case 2: If the PTM in an alteration is identified and localized, then the PTM
// information is stored in mod_ptr_. The amino acid in the mod_ptr_ will be the
// localized amino acid.

class Alter;
typedef std::shared_ptr<Alter> AlterPtr;

class Alter {
 public:
  Alter(int left_bp_pos, int right_bp_pos,
        AlterTypePtr type_ptr,
        double mass, ModPtr mod_ptr);

  explicit Alter(XmlDOMElement* change_element);

  int getLeftBpPos() {return left_bp_pos_;}

  void setLeftBpPos(int p) {left_bp_pos_ = p;}

  int getRightBpPos() {return right_bp_pos_;}

  void setRightBpPos(int p) {right_bp_pos_ = p;}

  AlterTypePtr getTypePtr() {return type_ptr_;}

  double getMass() {return mass_;}

  void setMass(double m) {mass_ = m;}

  ModPtr getModPtr() {return mod_ptr_;}

  LocalAnnoPtr getLocalAnno() {return local_anno_ptr_;}

  void setLocalAnno(LocalAnnoPtr p);

  void appendXml(XmlDOMDocument* xml_doc, XmlDOMElement* parent);

  static std::string getXmlElementName() {return "alteration";}

  static AlterPtr geneAlterPtr(AlterPtr ori_alter_ptr, int start_pos);

 protected:
  // left and right positions are based on break point positions
  int left_bp_pos_;

  int right_bp_pos_;

  AlterTypePtr type_ptr_;

  double mass_;

  ModPtr mod_ptr_;

  LocalAnnoPtr local_anno_ptr_;
};

typedef std::vector<AlterPtr> AlterPtrVec;

}  // namespace toppic

#endif

