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

#include <iostream>

#include <boost/thread/mutex.hpp>

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "seq/proteoform_util.hpp"
#include "seq/prot_candidate.hpp"
#include "ms/factory/prm_ms_util.hpp"

#include "filter/massmatch/mass_match_factory.hpp"
#include "filter/massmatch/mass_match_util.hpp"
#include "filter/oneptm/one_ptm_filter.hpp"

namespace toppic {

// serialization mutex.
boost::mutex one_ptm_filter_mutex;

OnePtmFilter::OnePtmFilter(const ProteoformPtrVec &proteo_ptrs,
                           OnePtmFilterMngPtr mng_ptr, std::string block_str) {
  mng_ptr_ = mng_ptr;
  proteo_ptrs_ = proteo_ptrs;
  PrsmParaPtr prsm_para_ptr = mng_ptr->prsm_para_ptr_;

  std::string index_dir = mng_ptr_->prsm_para_ptr_->getOriDbName() + "_idx";
  
  std::string parameters = mng_ptr->getIndexFilePara();
  std::string suffix = parameters + block_str;

  bool index_files_exist = true;

  for (size_t t = 0; t < mng_ptr->one_ptm_file_vec_.size(); t++){
    std::string file_name = mng_ptr->one_ptm_file_vec_[t] + suffix;
    if (!file_util::exists(index_dir + file_util::getFileSeparator() + file_name)){
      //if any of the index files for this ptm is missing
      index_files_exist = false;
      break; 
    }
  }

  if (index_files_exist){//if exists
    term_index_ptr_ = std::make_shared<MassMatch>();
    diag_index_ptr_ = std::make_shared<MassMatch>();
    rev_term_index_ptr_ = std::make_shared<MassMatch>();
    rev_diag_index_ptr_ = std::make_shared<MassMatch>();
    
    {
      boost::unique_lock<boost::mutex> lock(one_ptm_filter_mutex);
      //complete file name is a ptm type from topindexfile vector + parameters + db block id (block_str)
      term_index_ptr_->deserializeMassMatch(mng_ptr->one_ptm_file_vec_[0] + suffix, index_dir);
      diag_index_ptr_->deserializeMassMatch(mng_ptr->one_ptm_file_vec_[1] + suffix, index_dir);
      rev_term_index_ptr_->deserializeMassMatch(mng_ptr->one_ptm_file_vec_[2] + suffix, index_dir);
      rev_diag_index_ptr_->deserializeMassMatch(mng_ptr->one_ptm_file_vec_[3] + suffix, index_dir);
    }
  }
  else{
    std::vector<std::vector<double>> shift_2d
        = proteoform_util::getNTermShift2D(proteo_ptrs, mng_ptr->prsm_para_ptr_->getProtModPtrVec());
    std::vector<std::vector<double>> n_term_acet_2d
        = proteoform_util::getNTermAcet2D(proteo_ptrs, mng_ptr->prsm_para_ptr_->getProtModPtrVec());
    term_index_ptr_ = mass_match_factory::getPrmTermMassMatchPtr(proteo_ptrs, shift_2d,
                                                                 mng_ptr->max_proteoform_mass_,
                                                                 mng_ptr->filter_scale_);
    diag_index_ptr_ = mass_match_factory::getPrmDiagMassMatchPtr(proteo_ptrs,
                                                                 mng_ptr->max_proteoform_mass_,
                                                                 mng_ptr->filter_scale_);
    std::vector<std::vector<double>> rev_shift_2d;
    std::vector<double> shift_1d(1, 0);
    for (size_t i = 0; i < proteo_ptrs.size(); i++) {
      rev_shift_2d.push_back(shift_1d);
    }
    rev_term_index_ptr_ 
        = mass_match_factory::getSrmTermMassMatchPtr(proteo_ptrs, rev_shift_2d, n_term_acet_2d,
                                                     mng_ptr->max_proteoform_mass_,
                                                     mng_ptr->filter_scale_);
    rev_diag_index_ptr_ 
        = mass_match_factory::getSrmDiagMassMatchPtr(proteo_ptrs, n_term_acet_2d,
                                                     mng_ptr->max_proteoform_mass_,
                                                     mng_ptr->filter_scale_);
  };
}

void OnePtmFilter::computeBestMatch(const PrmMsPtrVec &prm_ms_ptr_vec,
                                    const PrmMsPtrVec &srm_ms_ptr_vec) {
  PeakTolerancePtr tole_ptr = mng_ptr_->prsm_para_ptr_->getSpParaPtr()->getPeakTolerancePtr();
  std::vector<std::pair<int, int>> pref_mass_errors
      = prm_ms_util::getIntMassErrorList(prm_ms_ptr_vec, tole_ptr, mng_ptr_->filter_scale_, true, false);
  std::vector<std::pair<int, int>> suff_mass_errors
      = prm_ms_util::getIntMassErrorList(srm_ms_ptr_vec, tole_ptr, mng_ptr_->filter_scale_, false, true);
  
  int term_row_num = term_index_ptr_->getRowNum();
  std::vector<short> term_scores(term_row_num, 0);
  term_index_ptr_->compScores(pref_mass_errors, term_scores);

  int diag_row_num = diag_index_ptr_->getRowNum();
  std::vector<short> diag_scores(diag_row_num, 0);
  diag_index_ptr_->compScores(pref_mass_errors, diag_scores);

  int rev_term_row_num = rev_term_index_ptr_->getRowNum();
  std::vector<short> rev_term_scores(rev_term_row_num, 0);
  rev_term_index_ptr_->compScores(suff_mass_errors, rev_term_scores);

  int rev_diag_row_num = rev_diag_index_ptr_->getRowNum();
  std::vector<short> rev_diag_scores(rev_diag_row_num, 0);
  rev_diag_index_ptr_->compScores(suff_mass_errors, rev_diag_scores);

  double prec_minus_water_mass = prm_ms_ptr_vec[0]->getMsHeaderPtr()->getFirstPrecMonoMassMinusWater();
  double prec_error_tole = prm_ms_ptr_vec[0]->getMsHeaderPtr()->getFirstPrecErrorTolerance(tole_ptr->getPpo());

  ProtCandidatePtrVec comp_prots
    = mass_match_util::findOneShiftTopProteins(term_scores, rev_term_scores, term_index_ptr_, rev_term_index_ptr_,
                                               prec_minus_water_mass, prec_error_tole, 
                                               mng_ptr_->min_shift_, mng_ptr_->max_shift_,
                                               mng_ptr_->comp_cand_shift_num_, mng_ptr_->threshold_, 
                                               mng_ptr_->comp_num_);
  comp_match_ptrs_.clear();
  int group_spec_num = prm_ms_ptr_vec.size();
  for (size_t i = 0; i < comp_prots.size(); i++) {
    int id = comp_prots[i]->getProteinId();
    int score = comp_prots[i]->getScore();
    comp_match_ptrs_.push_back(std::make_shared<SimplePrsm>(prm_ms_ptr_vec[0]->getMsHeaderPtr(),
                                                            group_spec_num,
                                                            proteo_ptrs_[id], score));
  }

  ProtCandidatePtrVec pref_prots
    = mass_match_util::findOneShiftTopProteins(term_scores, rev_diag_scores, term_index_ptr_, rev_diag_index_ptr_, 
                                               prec_minus_water_mass, prec_error_tole, 
                                               mng_ptr_->min_shift_, mng_ptr_->max_shift_, 
                                               mng_ptr_->cand_shift_num_, 
                                               mng_ptr_->threshold_, 
                                               mng_ptr_->pref_suff_num_); 
  pref_match_ptrs_.clear();
  for (size_t i = 0; i < pref_prots.size(); i++) {
    int id = pref_prots[i]->getProteinId();
    int score = pref_prots[i]->getScore();
    SimplePrsmPtr prsm_ptr = std::make_shared<SimplePrsm>(prm_ms_ptr_vec[0]->getMsHeaderPtr(),
                                                          group_spec_num,
                                                          proteo_ptrs_[id], score);
    std::vector<double> shifts = pref_prots[i]->getCTermShifts();
    prsm_ptr->setCTermShifts(pref_prots[i]->getCTermShifts());
    pref_match_ptrs_.push_back(prsm_ptr);
  }

  ProtCandidatePtrVec suff_prots
    = mass_match_util::findOneShiftTopProteins(diag_scores, rev_term_scores, diag_index_ptr_, rev_term_index_ptr_, 
                                               prec_minus_water_mass, prec_error_tole, 
                                               mng_ptr_->min_shift_, mng_ptr_->max_shift_,
                                               mng_ptr_->cand_shift_num_, 
                                               mng_ptr_->threshold_, 
                                               mng_ptr_->pref_suff_num_); 
  suff_match_ptrs_.clear();
  for (size_t i = 0; i < suff_prots.size(); i++) {
    int id = suff_prots[i]->getProteinId();
    int score = suff_prots[i]->getScore();
    SimplePrsmPtr prsm_ptr = std::make_shared<SimplePrsm>(prm_ms_ptr_vec[0]->getMsHeaderPtr(),
                                                          group_spec_num,
                                                          proteo_ptrs_[id], score);
    prsm_ptr->setNTermShifts(suff_prots[i]->getNTermShifts());
    suff_match_ptrs_.push_back(prsm_ptr);
  }

  ProtCandidatePtrVec internal_prots
    = mass_match_util::findOneShiftTopProteins(diag_scores, rev_diag_scores, 
                                               diag_index_ptr_, rev_diag_index_ptr_,
                                               prec_minus_water_mass, prec_error_tole, 
                                               mng_ptr_->min_shift_, mng_ptr_->max_shift_,
                                               mng_ptr_->cand_shift_num_,
                                               mng_ptr_->threshold_, mng_ptr_->inte_num_); 
  internal_match_ptrs_.clear();
  for (size_t i = 0; i < internal_prots.size(); i++) {
    int id = internal_prots[i]->getProteinId();
    int score = internal_prots[i]->getScore();
    SimplePrsmPtr prsm_ptr = std::make_shared<SimplePrsm>(prm_ms_ptr_vec[0]->getMsHeaderPtr(),
                                                          group_spec_num,
                                                          proteo_ptrs_[id], score);
    prsm_ptr->setNTermShifts(internal_prots[i]->getNTermShifts());
    prsm_ptr->setCTermShifts(internal_prots[i]->getCTermShifts());
    internal_match_ptrs_.push_back(prsm_ptr);
  }
}

} /* namespace toppic */
