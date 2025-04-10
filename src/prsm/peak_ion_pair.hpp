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

#ifndef TOPPIC_PRSM_PEAK_ION_PAIR_HPP_
#define TOPPIC_PRSM_PEAK_ION_PAIR_HPP_

#include <vector>

#include "common/xml/xml_dom_element.hpp"
#include "ms/spec/extend_peak.hpp"
#include "ms/spec/theo_peak.hpp"

namespace toppic {

class XmlDOMDocument;

class PeakIonPair;
typedef std::shared_ptr<PeakIonPair> PeakIonPairPtr;

// A pair of matched theoretical mass and experimental mass
class PeakIonPair {
 public:
  PeakIonPair(int spec_id, ExtendPeakPtr real_peak_ptr,
              TheoPeakPtr theo_peak_ptr);

  int getSpecId() {return spec_id_;}

  ExtendPeakPtr getRealPeakPtr() {return real_peak_ptr_;}

  TheoPeakPtr getTheoPeakPtr() {return theo_peak_ptr_;}

  void appendRealPeakToXml(XmlDOMDocument* xml_doc, 
                           XmlDOMElement* parent);

  void appendTheoPeakToXml(XmlDOMDocument* xml_doc, 
                           XmlDOMElement* parent);

  static bool cmpRealPeakPosInc(const PeakIonPairPtr &a, const PeakIonPairPtr &b);

  static bool cmpTheoPeakPosInc(const PeakIonPairPtr &a, const PeakIonPairPtr &b);

 private:
  int spec_id_;

  ExtendPeakPtr real_peak_ptr_;

  TheoPeakPtr theo_peak_ptr_;
};

typedef std::vector<PeakIonPairPtr> PeakIonPairPtrVec;
typedef std::vector<PeakIonPairPtrVec> PeakIonPairPtrVec2D;

}  // namespace toppic

#endif
