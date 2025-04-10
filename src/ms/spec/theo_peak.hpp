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

#ifndef TOPPIC_MS_SPEC_THEO_PEAK_HPP_
#define TOPPIC_MS_SPEC_THEO_PEAK_HPP_

#include "common/base/ion.hpp"
#include "ms/spec/peak.hpp"

namespace toppic {

class TheoPeak;
typedef std::shared_ptr<TheoPeak> TheoPeakPtr;

class TheoPeak : public Peak {
 public:
  TheoPeak(IonPtr ion_ptr, double unmod_mass, double shift);

  IonPtr getIonPtr() {return ion_ptr_;}

  double getModMass() {return getPosition();}

  double getShift() {return shift_;}

  static bool cmpPosInc(const TheoPeakPtr &a, const TheoPeakPtr &b) {
    return a->getPosition() < b->getPosition();
}

 private:
  IonPtr ion_ptr_;
  double shift_;
};

typedef std::vector<TheoPeakPtr> TheoPeakPtrVec;

} /* namespace toppic */

#endif /* THEO_PEAK_HPP_ */
