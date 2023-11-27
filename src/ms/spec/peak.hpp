//Copyright (c) 2014 - 2023, The Trustees of Indiana University.
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

#ifndef TOPPIC_MS_SPEC_PEAK_HPP_
#define TOPPIC_MS_SPEC_PEAK_HPP_

#include <memory>
#include <vector>
#include <string>

#include "common/xml/xml_dom_element.hpp"

namespace toppic {

// Use forword declaration instead of header files
// to speed up compilation
class XmlDOMDocument;

class Peak;

typedef std::shared_ptr<Peak> PeakPtr;
typedef std::vector<PeakPtr> PeakPtrVec;

class Peak {
 public:
  Peak(double position, double intensity);

  double getIntensity() const {return intensity_;}

  double getPosition() const {return position_;}

  void setIntensity(double intensity) {intensity_ = intensity;}

  void setPosition(double position) {position_ = position;}

  void changeIntensity(double ratio) {intensity_ = intensity_ * ratio;}

  void shiftPosition(double shift) {position_ = position_ + shift;}

  void appendXml(XmlDOMDocument* xml_doc, XmlDOMElement* parent);

  static std::string getXmlElementName() {return "peak";}

  static bool cmpInteDec(const PeakPtr &a, const PeakPtr &b) { 
    return a->getIntensity() > b->getIntensity();}

  static bool cmpPosInc(const PeakPtr &a, const PeakPtr &b) { 
    return a->getPosition() < b->getPosition();}

private:
  double position_ = -1;
  double intensity_ = -1;
};

typedef std::vector<PeakPtrVec> PeakPtrVec2D;

}  // namespace toppic
#endif
