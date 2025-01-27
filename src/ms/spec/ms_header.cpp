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

#include <cmath>
#include <sstream>

#include "common/util/logger.hpp"
#include "common/util/str_util.hpp"
#include "common/xml/xml_dom_document.hpp"
#include "common/xml/xml_dom_util.hpp"
#include "common/base/activation_base.hpp"
#include "ms/spec/peak_util.hpp"
#include "ms/spec/ms_header.hpp"

namespace toppic {

MsHeader::MsHeader(XmlDOMElement* element) {
  file_name_ = xml_dom_util::getChildValue(element, "file_name", 0);
  spec_id_ = xml_dom_util::getIntChildValue(element, "spec_id", 0);
  title_ = xml_dom_util::getChildValue(element, "title", 0);
  level_ = xml_dom_util::getIntChildValue(element, "level", 0);

  XmlDOMElement* scan_element
      = xml_dom_util::getChildElement(element, "scan_list", 0);
  int scans = xml_dom_util::getChildCount(scan_element, "scan");
  for (int i = 0; i < scans; i++) {
    scans_.push_back(xml_dom_util::getIntChildValue(scan_element, "scan", i));
  }
  retention_time_ = xml_dom_util::getDoubleChildValue(element, "retention_time", 0);
  prec_target_mz_ = xml_dom_util::getDoubleChildValue(element, "prec_target_mz", 0);
  std::string element_name = Activation::getXmlElementName();
  XmlDOMElement* ac_element
      = xml_dom_util::getChildElement(element, element_name.c_str(), 0);
  activation_ptr_ = ActivationBase::getActivationPtrFromXml(ac_element);
}

std::string MsHeader::toString() {
  std::stringstream tmp;
  tmp << "MS HEADER\n";
  tmp << "==========\n";
  tmp << "Title = " << title_ << "\n";
  tmp << "Scan Number = " << scans_[0] << "\n";
  tmp << "MS Level = " << level_ << "\n";
  tmp << "Activation type = " << activation_ptr_ << "\n";
  tmp << "Precursor Target Mz = " << prec_target_mz_ << "\n";
  return tmp.str();
}

std::string MsHeader::getScansString() {
  std::stringstream scan_list;
  scan_list << scans_[0];
  for (size_t i = 1; i < scans_.size(); i++) {
    scan_list <<  " " << scans_[i];
  }
  return scan_list.str();
}

PrecursorPtr MsHeader::getFirstPrecPtr() {
  if (prec_ptr_vec_.size() > 0)  {
    return prec_ptr_vec_[0];
  }
  else {
    LOG_ERROR("The MS/MS scan does not contain precursor information!");
    exit(EXIT_FAILURE);
  }
}

void MsHeader::setScans(const std::string &s) {
  if (s == "") {
    scans_.clear();
    scans_.push_back(-1);
    return;
  }
  std::vector<std::string> strs = str_util::split(s, " ");
  for (size_t i = 0; i < strs.size(); i++) {
    scans_.push_back(std::stoi(strs[i]));
  }
}

void MsHeader::setSingleScan(int scan_num) {
  scans_.clear(); 
  scans_.push_back(scan_num);
}

void MsHeader::setSinglePrecPtr(PrecursorPtr prec_ptr) {
  prec_ptr_vec_.clear(); 
  prec_ptr_vec_.push_back(prec_ptr); 
}

XmlDOMElement* MsHeader::getHeaderXml(XmlDOMDocument* xml_doc) {
  XmlDOMElement* element = xml_doc->createElement("ms_header");
  xml_doc->addElement(element, "file_name", file_name_.c_str());
  std::string str = str_util::toString(spec_id_);
  xml_doc->addElement(element, "spec_id", str.c_str());
  xml_doc->addElement(element, "title", title_.c_str());
  str = str_util::toString(level_);
  xml_doc->addElement(element, "level", str.c_str());
  str = getScansString();
  xml_doc->addElement(element, "scans", str.c_str());
  XmlDOMElement* scans = xml_doc->createElement("scan_list");
  for (size_t i = 0; i < scans_.size(); i++) {
    str = str_util::toString(scans_[i]);
    xml_doc->addElement(scans, "scan", str.c_str());
  }
  element->appendChild(scans);
  str = str_util::toString(retention_time_);
  xml_doc->addElement(element, "retention_time", str.c_str());
  str = str_util::toString(prec_target_mz_);
  xml_doc->addElement(element, "prec_target_mz", str.c_str());
  activation_ptr_->appendNameToXml(xml_doc, element);
  return element;
}

void MsHeader::appendXml(XmlDOMDocument* xml_doc, XmlDOMElement* parent) {
  parent->appendChild(getHeaderXml(xml_doc));
}

MsHeaderPtr MsHeader::geneMsHeaderPtr(MsHeaderPtr ori_header_ptr, double new_prec_mass) {
  PrecursorPtr ori_prec_ptr = ori_header_ptr->getFirstPrecPtr();
  PrecursorPtr new_prec_ptr = std::make_shared<Precursor>(*ori_prec_ptr.get());
  double mono_mz = peak_util::compMz(new_prec_mass, ori_prec_ptr->getCharge());
  new_prec_ptr->setMonoMz(mono_mz);
  MsHeaderPtr new_header_ptr = std::make_shared<MsHeader>(*ori_header_ptr.get());
  new_header_ptr->setSinglePrecPtr(new_prec_ptr);
  return new_header_ptr;
}

bool MsHeader::cmpPrecInteDec(const MsHeaderPtr &a, const MsHeaderPtr &b) {
  return a->getFirstPrecInte() > b->getFirstPrecInte();
}

int MsHeader::getFirstPrecId() {
  return getFirstPrecPtr()->getPrecId();
}

double MsHeader::getFirstPrecMonoMz() {
  return getFirstPrecPtr()->getMonoMz();
}

int MsHeader::getFirstPrecCharge() {
  return getFirstPrecPtr()->getCharge();
}

double MsHeader::getFirstPrecInte() {
  return getFirstPrecPtr()->getInte();
}

double MsHeader::getFirstPrecMonoMass() {
  return getFirstPrecPtr()->getMonoMass();
}

int MsHeader::getFirstPrecFeatureId() {
  return getFirstPrecPtr()->getFeatureId();
}

double MsHeader::getFirstPrecMonoMassMinusWater() {
  return getFirstPrecPtr()->getMonoMassMinusWater();
}

double MsHeader::getFirstPrecErrorTolerance(double ppo) {
  return getFirstPrecPtr()->getErrorTolerance(ppo);
}

std::pair<int,int> MsHeader::getFirstPrecMonoMassMinusWaterError(double ppo, double scale) {
  return getFirstPrecPtr()->getMonoMassMinusWaterError(ppo, scale);
}

}  // namespace toppic
