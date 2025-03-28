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

#include <ctime>
#include <iostream>

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"
#include "common/util/str_util.hpp"
#include "ms/spec/deconv_ms.hpp"
#include "ms/spec/ms_header.hpp"
#include "ms/spec/msalign_reader_util.hpp"
#include "ms/mzml/mzml_ms_group_reader.hpp"
#include "ms/feature/spec_feature_writer.hpp"
#include "ms/feature/frac_feature_writer.hpp"
#include "ms/msmap/ms_map.hpp"
#include "topfd/common/topfd_para.hpp"
#include "topfd/ecscore/para/ecscore_para.hpp"
#include "topfd/ecscore/env/seed_env.hpp"
#include "topfd/ecscore/env/seed_env_util.hpp"
#include "topfd/ecscore/env_coll/env_coll.hpp"
#include "topfd/ecscore/env_coll/env_coll_util.hpp"
#include "topfd/ecscore/score/ecscore.hpp"
#include "topfd/ecscore/score/ecscore_writer.hpp"
#include "topfd/ecscore/env_coll/env_coll_assign.hpp"
#include "topfd/ecscore/env_coll/env_coll_detect.hpp"

namespace toppic {

namespace env_coll_detect {

void processMs1(TopfdParaPtr topfd_para_ptr) {
  if (topfd_para_ptr->isMissingLevelOne()) {
    return;
  }
  
  EcscoreParaPtr score_para_ptr = std::make_shared<EcscorePara>(topfd_para_ptr->getFracId(), 
                                                                topfd_para_ptr->getMzmlFileName(),
                                                                topfd_para_ptr->getMaxCharge(),
                                                                topfd_para_ptr->getMs1MinScanNum());
  // read deconvoluted MS1 peaks
  std::string output_base_name = topfd_para_ptr->getOutputBaseName();
  std::string ms1_file_name = output_base_name + "_ms1.msalign";
  DeconvMsPtrVec deconv_ms1_ptr_vec;
  msalign_reader_util::readAllSpectra(ms1_file_name, deconv_ms1_ptr_vec);

  // read ms1 raw peaks and ms2_headers
  PeakPtrVec2D ms1_mzml_peaks;
  MsHeaderPtr2D ms2_header_ptr_2d;
  MzmlMsGroupReaderPtr mzml_reader_ptr = 
    std::make_shared<MzmlMsGroupReader>(topfd_para_ptr->getMzmlFileName(), 
                                        topfd_para_ptr->getPrecWindowWidth(),
                                        topfd_para_ptr->getActivation(),
                                        topfd_para_ptr->getFracId(),
                                        topfd_para_ptr->isFaims(), 
                                        topfd_para_ptr->getFaimsVoltage(), 
                                        topfd_para_ptr->isMissingLevelOne());
  mzml_reader_ptr->getMs1Map(ms1_mzml_peaks, ms2_header_ptr_2d); 
  mzml_reader_ptr = nullptr;

  //Prepare seed envelopes
  SeedEnvPtrVec seed_ptrs;
  SeedEnvPtr2D seed_ptr_2d;
  for (auto &ms1_data: deconv_ms1_ptr_vec) {
    DeconvPeakPtrVec peaks = ms1_data->getPeakPtrVec();
    SeedEnvPtrVec one_spec_seed_ptrs;
    for (auto &peak: peaks) {
      SeedEnvPtr seed_ptr_1 = std::make_shared<SeedEnv>(peak);
      seed_ptrs.push_back(seed_ptr_1);
      SeedEnvPtr seed_ptr_2 = std::make_shared<SeedEnv>(peak);
      one_spec_seed_ptrs.push_back(seed_ptr_2);
    }
    seed_ptr_2d.push_back(one_spec_seed_ptrs);
  }

  std::sort(seed_ptrs.begin(), seed_ptrs.end(), SeedEnv::cmpSeedInteDec);
  // write_out_files::write_seed_envelopes(seed_envs, "envs.csv");

  double sn_ratio = topfd_para_ptr->getMsOneSnRatio();
  bool single_scan_noise = topfd_para_ptr->isUseSingleScanNoiseLevel();
  /// Prepare data -- Peak Matrix
  MsMapPtr matrix_ptr = std::make_shared<MsMap>(ms1_mzml_peaks, deconv_ms1_ptr_vec,
                                                score_para_ptr->bin_size_,
                                                sn_ratio, single_scan_noise);

  if (score_para_ptr->min_scan_num_ >= 2) {
    matrix_ptr->removeNonNeighbors(score_para_ptr->neighbor_mz_tole_);
  }

  /// Extract Fetures
  LOG_DEBUG("Number of seed envelopes: " << seed_ptrs.size());
  int seed_num = seed_ptrs.size();
  EnvCollPtrVec env_coll_list;
  ECScorePtrVec ecscore_list;
  for (int seed_env_idx = 0; seed_env_idx < seed_num; seed_env_idx++) {
    int count = seed_env_idx + 1;
    if (count % 100 == 0 || count == seed_num) {
      double perc = static_cast<int>(count * 100 / seed_num);
      std::cout << "\r" << "Processing feature " << count << " ...       " << perc << "\% finished." << std::flush;
    }
    SeedEnvPtr seed_ptr = seed_ptrs[seed_env_idx];
    seed_ptr = seed_env_util::preprocessSeedEnvPtr(seed_ptr, matrix_ptr,  
                                                   score_para_ptr, sn_ratio); 
    if (seed_ptr == nullptr) continue;
    EnvCollPtr env_coll_ptr = env_coll_util::findEnvColl(matrix_ptr, seed_ptr,
                                                         score_para_ptr, sn_ratio, 
                                                         topfd_para_ptr->getSplitIntensityRatio());
    if (env_coll_ptr == nullptr) continue;
    if (env_coll_util::checkExistingFeatures(matrix_ptr, env_coll_ptr,
                                             env_coll_list, score_para_ptr, sn_ratio)) {
      env_coll_ptr->removePeakData(matrix_ptr);
      continue;
    }
    env_coll_ptr->refineMonoMass();
    ECScorePtr ecscore_ptr = std::make_shared<ECScore>(env_coll_ptr, matrix_ptr,
                                                       sn_ratio); 
    if (ecscore_ptr->getScore() < topfd_para_ptr->getMs1EcscoreCutoff()) {
      continue;
    }
    env_coll_ptr->setEcscore(ecscore_ptr->getScore());
    env_coll_ptr->removePeakData(matrix_ptr);
    env_coll_list.push_back(env_coll_ptr);
    ecscore_list.push_back(ecscore_ptr);
  }
  std::cout << std::endl; 

  if (topfd_para_ptr->isSearchPrecWindow()) {
    // add ms1 feature based on precursor windows
    // set min match envelope to 1 to accept single scan features
    score_para_ptr->min_scan_num_ = 1;
    score_para_ptr->min_match_peak_ = 1;
    sn_ratio = 0;
    matrix_ptr->reconstruct(sn_ratio, single_scan_noise); 
    int ms1_spec_num = deconv_ms1_ptr_vec.size();
    for (std::size_t ms1_idx = 0; ms1_idx < deconv_ms1_ptr_vec.size(); ms1_idx++) {
      double perc = static_cast<int>((ms1_idx + 1)* 100 / ms1_spec_num);
      int scan = deconv_ms1_ptr_vec[ms1_idx]->getMsHeaderPtr()->getFirstScanNum();
      std::cout << "\r" << "Additional feature search MS1 spectrum scan " 
        << scan << " ...       " << perc << "\% finished." << std::flush;
      for (std::size_t i = 0; i < ms2_header_ptr_2d[ms1_idx].size(); i++) {
        MsHeaderPtr header_ptr = ms2_header_ptr_2d[ms1_idx][i];
        if (env_coll_assign::checkEnvColl(header_ptr, env_coll_list)) {
          continue;
        }
        double prec_win_begin = header_ptr->getPrecWinBegin();
        double prec_win_end = header_ptr->getPrecWinEnd();
        SeedEnvPtrVec seed_ptr_list = seed_ptr_2d[ms1_idx];
        SeedEnvPtrVec selected_seed_list;
        LOG_DEBUG("ms1 id  " << ms1_idx << " seed number " << seed_ptr_list.size());
        for (std::size_t i = 0; i < seed_ptr_list.size(); i++) {
          double ref_mz = seed_ptr_list[i]->getReferMz();
          if (ref_mz > prec_win_begin && ref_mz < prec_win_end) {
            selected_seed_list.push_back(seed_ptr_list[i]);
          }
        }
        SeedEnvPtr seed_ptr;
        if (selected_seed_list.size() > 0) {
          // choose the highest intensity one
          std::sort(selected_seed_list.begin(), selected_seed_list.end(),
                    SeedEnv::cmpSeedInteDec);
          seed_ptr = selected_seed_list[0];  
          seed_ptr = seed_env_util::relaxProcessSeedEnvPtr(seed_ptr, matrix_ptr,  
                                                           score_para_ptr, sn_ratio); 
        }
        if (seed_ptr == nullptr) {
          continue;
        }

        EnvCollPtr env_coll_ptr = env_coll_util::findEnvCollWithSingleEnv(matrix_ptr, seed_ptr, score_para_ptr,
                                                                          sn_ratio, topfd_para_ptr->getSplitIntensityRatio());
        if (env_coll_ptr == nullptr) continue;
        if (env_coll_util::checkExistingFeatures(matrix_ptr, env_coll_ptr,
                                                 env_coll_list, score_para_ptr, sn_ratio)) {
          env_coll_ptr->removePeakData(matrix_ptr);
          continue;
        }
        env_coll_ptr->refineMonoMass();
        ECScorePtr ecscore_ptr = std::make_shared<ECScore>(env_coll_ptr, matrix_ptr,
                                                           sn_ratio); 
        if (ecscore_ptr->getScore() < 0 || std::isnan(ecscore_ptr->getScore())) {
          continue;
        }
        env_coll_ptr->setEcscore(ecscore_ptr->getScore());
        env_coll_ptr->removePeakData(matrix_ptr);
        env_coll_list.push_back(env_coll_ptr);
        ecscore_list.push_back(ecscore_ptr);
      }
    }
    std::cout << std::endl; 
  }

  FracFeaturePtrVec frac_features;
  for (std::size_t i = 0; i < env_coll_list.size(); i++) {
    EnvCollPtr env_coll_ptr = env_coll_list[i];
    FracFeaturePtr frac_feat_ptr = env_coll_util::getFracFeature(i, deconv_ms1_ptr_vec, 
                                                                 score_para_ptr->frac_id_,
                                                                 score_para_ptr->file_name_,
                                                                 env_coll_ptr, matrix_ptr, sn_ratio);
    frac_features.push_back(frac_feat_ptr);
  }

  // map MS2 features
  SpecFeaturePtrVec ms2_features;
  env_coll_assign::assignEnvColls(frac_features, env_coll_list, ms2_header_ptr_2d,
                                  ms2_features, topfd_para_ptr->getMs1EcscoreCutoff()); 
  std::cout << "Number of proteoform features: " << env_coll_list.size() << std::endl;
  /// output files
  if (topfd_para_ptr->isOutputCsvFeatureFile()) {
    std::string feat_file_name = output_base_name + "_ms1.csv";
    ecscore_writer::writeScores(feat_file_name, ecscore_list);
    std::string batmass_file_name = output_base_name + "_" + "frac_ms1.mzrt.csv";
    int num_spec = ms1_mzml_peaks.size();
    frac_feature_writer::writeBatMassFeatures(batmass_file_name, frac_features, num_spec);
  }

  std::string frac_feat_xml_file_name = output_base_name + "_feature.xml";
  frac_feature_writer::writeXmlFeatures(frac_feat_xml_file_name, frac_features);
  std::string frac_feat_file_name = output_base_name + "_" + "ms1.feature";
  frac_feature_writer::writeFeatures(frac_feat_file_name, frac_features);

  //SampleFeaturePtrVec sample_features;
  //feature_util::getSampleFeatures(sample_features, frac_features, ms2_features);
  //std::string sample_feature_file_name = output_base_name + "_"  + "ms1.feature";
  //sample_feature_writer::writeFeatures(sample_feature_file_name, sample_features);
  std::string ms2_feat_file_name = output_base_name + "_"  + "ms2.feature";
  spec_feature_writer::writeFeatures(ms2_feat_file_name, ms2_features);
}

void processMs2(TopfdParaPtr topfd_para_ptr) {
  EcscoreParaPtr score_para_ptr = std::make_shared<EcscorePara>(topfd_para_ptr->getFracId(),
                                                                topfd_para_ptr->getMzmlFileName(),
                                                                topfd_para_ptr->getMaxCharge(),
                                                                topfd_para_ptr->getMs2MinScanNum());
                                                      
  // read deconvoluted MS2 peaks
  std::string output_base_name = topfd_para_ptr->getOutputBaseName();
  std::string ms2_file_name = output_base_name + "_ms2_raw.msalign";
  DeconvMsPtrVec deconv_ms2_ptr_vec;
  msalign_reader_util::readAllSpectra(ms2_file_name, deconv_ms2_ptr_vec);

  // get isolation window 
  std::set<std::pair<double,double>> win_set;
  for (auto &ms2_data: deconv_ms2_ptr_vec) {
    if (ms2_data->getMsHeaderPtr()->getMsLevel() == 1)
      continue;
    std::pair<double, double> cur_win(ms2_data->getMsHeaderPtr()->getPrecWinBegin(), 
                                      ms2_data->getMsHeaderPtr()->getPrecWinEnd()); 
    win_set.insert(cur_win);
  }
  std::vector<std::pair<double,double>> win_list(win_set.begin(), win_set.end()); 

  for (std::size_t i = 0; i < win_list.size(); i++) {
  //for (size_t i = 0; i < 1; i++) {
    std::pair<double, double> cur_win =  win_list[i]; 
    std::cout << "Processing isolation window: [" << cur_win.first << "," << cur_win.second << "]"<< std::endl;
    // read ms1 raw peaks and ms2_headers
    PeakPtrVec2D ms2_mzml_peaks;
    
    MzmlMsGroupReaderPtr mzml_reader_ptr =
      std::make_shared<MzmlMsGroupReader>(topfd_para_ptr->getMzmlFileName(),
                                          topfd_para_ptr->getPrecWindowWidth(),
                                          topfd_para_ptr->getActivation(),
                                          topfd_para_ptr->getFracId(),
                                          topfd_para_ptr->isFaims(),
                                          topfd_para_ptr->getFaimsVoltage(),
                                          topfd_para_ptr->isMissingLevelOne());
    mzml_reader_ptr->getMs2Map(ms2_mzml_peaks, cur_win.first);
    mzml_reader_ptr = nullptr;

    //Prepare seed envelopes
    int spec_id = 0;
    DeconvMsPtrVec deconv_ms2_ptr_shortlisted_vec;
    SeedEnvPtrVec seed_ptrs;
    for (auto &ms2_data: deconv_ms2_ptr_vec) {
      double ms_win_begin = ms2_data->getMsHeaderPtr()->getPrecWinBegin(); 
      if (ms_win_begin != cur_win.first)
        continue;
      deconv_ms2_ptr_shortlisted_vec.push_back(ms2_data);
      DeconvPeakPtrVec peaks = ms2_data->getPeakPtrVec();
      for (auto &peak: peaks) {
        peak->setSpId(spec_id);
        SeedEnvPtr seed_ptr_1 = std::make_shared<SeedEnv>(peak);
        seed_ptrs.push_back(seed_ptr_1);
      }
      spec_id = spec_id + 1;
    }

    std::sort(seed_ptrs.begin(), seed_ptrs.end(), SeedEnv::cmpSeedInteDec);

    double sn_ratio = topfd_para_ptr->getMsTwoSnRatio();
    bool single_scan_noise = topfd_para_ptr->isUseSingleScanNoiseLevel();
    /// Prepare data -- Peak Matrix
    MsMapPtr matrix_ptr = std::make_shared<MsMap>(ms2_mzml_peaks, deconv_ms2_ptr_shortlisted_vec,
                                                  score_para_ptr->bin_size_,
                                                  topfd_para_ptr->getMsTwoSnRatio(), single_scan_noise);

    if (score_para_ptr->min_scan_num_ > 2) {
      matrix_ptr->removeNonNeighbors(score_para_ptr->neighbor_mz_tole_);
    }

    /// Extract Fetures
    LOG_DEBUG("Number of seed envelopes: " << seed_ptrs.size());
    int seed_num = static_cast<int>(seed_ptrs.size());
    EnvCollPtrVec env_coll_list;
    ECScorePtrVec ecscore_list;
    for (int seed_env_idx = 0; seed_env_idx < seed_num; seed_env_idx++) {
      int count = seed_env_idx + 1;
      if (count % 100 == 0 || count == seed_num) {
        double perc = static_cast<int>(count * 100 / seed_num);
        std::cout << "\r" << "Processing feature " << count << " ...       " << perc << "\% finished." << std::flush;
      }
      SeedEnvPtr seed_ptr = seed_ptrs[seed_env_idx];
      seed_ptr = seed_env_util::preprocessSeedEnvPtr(seed_ptr, matrix_ptr, score_para_ptr, sn_ratio);
      if (seed_ptr == nullptr) continue;
      EnvCollPtr env_coll_ptr = env_coll_util::findEnvColl(matrix_ptr, seed_ptr, score_para_ptr, sn_ratio,
                                                           topfd_para_ptr->getSplitIntensityRatio());
      if (env_coll_ptr == nullptr) continue;
      if (env_coll_util::checkExistingFeatures(matrix_ptr, env_coll_ptr, env_coll_list, score_para_ptr, sn_ratio)) {
        env_coll_ptr->removePeakData(matrix_ptr);
        continue;
      }
      env_coll_ptr->refineMonoMass();
      ECScorePtr ecscore_ptr = std::make_shared<ECScore>(env_coll_ptr, matrix_ptr, topfd_para_ptr->getMsTwoSnRatio());
      if (ecscore_ptr->getScore() < topfd_para_ptr->getMs2EcscoreCutoff()) {
        continue;
      }
      ecscore_list.push_back(ecscore_ptr);
      env_coll_ptr->setEcscore(ecscore_ptr->getScore());
      env_coll_ptr->removePeakData(matrix_ptr);
      env_coll_list.push_back(env_coll_ptr);
    }
    std::cout << std::endl;

    std::cout << "Number of fragment features: " << env_coll_list.size() << std::endl;

    FracFeaturePtrVec frac_features;
    for (std::size_t i = 0; i < env_coll_list.size(); i++) {
      EnvCollPtr env_coll_ptr = env_coll_list[i];
      FracFeaturePtr frac_feat_ptr = env_coll_util::getFracFeature(i, deconv_ms2_ptr_shortlisted_vec,
                                                                   score_para_ptr->frac_id_,
                                                                   score_para_ptr->file_name_,
                                                                   env_coll_ptr, matrix_ptr, sn_ratio);
      frac_features.push_back(frac_feat_ptr);
    }

    std::string feat_file_name =
        output_base_name + "_" + std::to_string(cur_win.first) + "_ms2.csv";
    ecscore_writer::writeScores(feat_file_name, ecscore_list);
    /// output files
    if (topfd_para_ptr->isOutputCsvFeatureFile()) {
      std::string batmass_file_name;
      batmass_file_name =
        output_base_name + "_" + std::to_string(cur_win.first) + "_frac_ms2.mzrt.csv";
      int num_spec = ms2_mzml_peaks.size();
      frac_feature_writer::writeBatMassFeatures(batmass_file_name, frac_features, num_spec);
    }
  }
}

}  // namespace

}  // namespace toppic
