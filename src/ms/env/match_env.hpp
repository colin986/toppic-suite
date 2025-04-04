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

#ifndef TOPPIC_TOPFD_ENV_MATCH_ENVELOPE_HPP_
#define TOPPIC_TOPFD_ENV_MATCH_ENVELOPE_HPP_

#include "ms/env/env_para.hpp"
#include "ms/env/env.hpp"
#include "ms/env/exp_env.hpp"

namespace toppic {

class MatchEnv;

typedef std::shared_ptr<MatchEnv> MatchEnvPtr;

class MatchEnv {
 public:
  MatchEnv(int mass_group, EnvPtr theo_env_ptr,
           ExpEnvPtr real_env_ptr);

  int getId() {return id_;}

  int getMassGroup() {return mass_group_;}

  ExpEnvPtr getExpEnvPtr() {return exp_env_ptr_;}

  EnvPtr getTheoEnvPtr() {return theo_env_ptr_;}

  double getEnvcnnScore() {return envcnn_score_;}

  void setEnvcnnScore(double score) {envcnn_score_ = score;}

  double getMsdeconvScore() {return msdeconv_score_;}

  void setMsdeconvScore(double score) {msdeconv_score_ = score;}

  void setId(int id) {id_ = id;}

  void setTheoEnvPtr(EnvPtr theo_env_ptr) { theo_env_ptr_ = theo_env_ptr;}

  void compMsdeconvScr(EnvParaPtr env_para_ptr);

  static bool cmpEnvcnnScoreDec(const MatchEnvPtr &a, const MatchEnvPtr &b) { 
    return a->getEnvcnnScore() > b->getEnvcnnScore();}

  static bool cmpMsdeconvScoreDec(const MatchEnvPtr &a, const MatchEnvPtr &b) { 
    return a->getMsdeconvScore() > b->getMsdeconvScore();}

  double calcPeakScr(int id_x, double inte_sum, double tolerance);

  void appendXml(XmlDOMDocument* xml_doc,xercesc::DOMElement* parent);

  static std::string getXmlElementName() {return "match_env";}

 private:
  int id_;
  // we divide envelopes into several groups based on monoisotopic masses  
  int mass_group_;
  double msdeconv_score_;
  double envcnn_score_;
  EnvPtr theo_env_ptr_;
  ExpEnvPtr exp_env_ptr_;

  double calcShareInteAccu(int id_x, double inte_sum);

  double calcMzFactor(int id_x, double shift, double tolerance);

  double calcIntensityFactor(double theo_inte, double real_inte);

  double calcIntensityFactor(int id_x, double ratio);

  double findBestShift(EnvParaPtr env_para_ptr);

  double findBestRatio(EnvParaPtr env_para_ptr);

  double calcScrWithSftRatio(double shift, double ratio, double tolerance);
};

typedef std::vector<MatchEnvPtr> MatchEnvPtrVec;
typedef std::vector<MatchEnvPtrVec> MatchEnvPtr2D;

}  // namespace toppic

#endif
