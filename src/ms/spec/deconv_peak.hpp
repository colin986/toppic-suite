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

#ifndef TOPPIC_MS_SPEC_DECONV_PEAK_HPP_
#define TOPPIC_MS_SPEC_DECONV_PEAK_HPP_

#include "ms/spec/peak_util.hpp"
#include "ms/spec/peak.hpp"

namespace toppic {

class DeconvPeak;
typedef std::shared_ptr<DeconvPeak> DeconvPeakPtr;

class DeconvPeak : public Peak {
 public:
  DeconvPeak(int sp_id, int peak_id, double mono_mass, 
             double intensity, int charge);

  DeconvPeak(int sp_id, int peak_id, double mono_mass, 
             double intensity, int charge, double score);

  explicit DeconvPeak(XmlDOMElement* element);

  int getCharge() {return charge_;}

  int getSpId() {return sp_id_;}

  int getPeakId() {return peak_id_;}

  double getMonoMass() {return getPosition();}

  double getMonoMz() {return peak_util::compMz(getMonoMass(), charge_);}

  double getScore() {return score_;}

  void setSpId(int sp_id) {sp_id_ = sp_id;}

  void setPeakId(int peak_id) {peak_id_ = peak_id;}

  void appendXml(XmlDOMDocument* xml_doc, XmlDOMElement* parent);

  static bool cmpPosInc(const DeconvPeakPtr &a, const DeconvPeakPtr &b) {
    return a->getPosition() < b->getPosition();}

  static std::string getXmlElementName() {return "deconv_peak";}

 private:
  int sp_id_;
  int peak_id_;
  int charge_;
  double score_ = 1.0;
};

typedef std::vector<DeconvPeakPtr> DeconvPeakPtrVec;
typedef std::vector<DeconvPeakPtrVec> DeconvPeakPtrVec2D;

}  // namespace toppic

#endif
