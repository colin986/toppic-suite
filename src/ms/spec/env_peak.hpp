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

#ifndef TOPPIC_MS_SPEC_ENV_PEAK_HPP_
#define TOPPIC_MS_SPEC_ENV_PEAK_HPP_

#include "ms/spec/peak.hpp"

namespace toppic {

class EnvPeak;
typedef std::shared_ptr<EnvPeak> EnvPeakPtr;

class EnvPeak : public Peak {
 public:
  EnvPeak(double mz, double intensity);

  EnvPeak(double mz, double intensity, int idx);

  EnvPeak(EnvPeakPtr peak_ptr);

  EnvPeak(XmlDOMElement* element);

  int getIdx() {return idx_;}

  void setIdx(int idx) {idx_ = idx;}

  bool isExist();

  void appendToXml(XmlDOMDocument* xml_doc, XmlDOMElement* parent);

  static int getNonExistPeakIdx() {return -1;}

  static bool cmpPosInc(const EnvPeakPtr &a, const EnvPeakPtr &b);

  static bool cmpInteInc(const EnvPeakPtr &a, const EnvPeakPtr &b);

  static std::string getXmlElementName() {return "env_peak";}

 private:
  int idx_;
};

typedef std::vector<EnvPeakPtr> EnvPeakPtrVec;

}  // namespace toppic

#endif
