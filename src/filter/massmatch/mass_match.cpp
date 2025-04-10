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
#include <iostream>
#include <fstream>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/serialization/vector.hpp>

#include "common/util/logger.hpp"
#include "common/util/file_util.hpp"

#include "filter/massmatch/mass_match.hpp"

namespace toppic {

/* proteo_minus_water_masses[i]: the mass (minus water) of the ith proteoform
 * mass_2d[i]: a vector containing prefix residue masses of the ith proteoform 
 * float_shift_2d[i]: a vector containing all possible shifts of the ith proteoform
 * pos_2d[i]: a vector containing the first residue position for each shift. 
 */
MassMatch::MassMatch(std::vector<double> &proteo_minus_water_masses,
                     std::vector<std::vector<int>> &mass_2d,
                     std::vector<std::vector<double>> &float_shift_2d,
                     std::vector<std::vector<int>> &pos_2d,
                     double max_proteoform_mass, double scale) { 
  scale_ = scale;
  LOG_DEBUG("Scale: " << scale_);

  proteo_num_ = proteo_minus_water_masses.size();
  LOG_DEBUG("Proteoform number: " << proteo_minus_water_masses.size());
  proteo_minus_water_masses_ = proteo_minus_water_masses;

  col_num_ = max_proteoform_mass * scale_;
  LOG_DEBUG("column number: " << col_num_);

  LOG_DEBUG("start init");
  initProteoformBeginEnds(float_shift_2d);
  LOG_DEBUG("row number: " << row_num_);

  LOG_DEBUG("init indexes");
  initIndexes(mass_2d, float_shift_2d, pos_2d);
}

void MassMatch::serializeMassMatch(std::string file_name, std::string dir_name){
  std::string file_path = dir_name + file_util::getFileSeparator() + file_name;
  std::ofstream new_file(file_path, std::ofstream::binary);

  if(new_file.is_open()) {
    boost::archive::binary_oarchive oa(new_file, std::ios::binary);
    oa << scale_;
    oa << proteo_num_;
    oa << col_num_; 
    oa << row_num_;
    
    oa << proteo_row_begins_;
    oa << proteo_row_ends_;
    oa << proteo_minus_water_masses_;
    oa << row_proteo_ids_;
    oa << trunc_shifts_;

    oa << col_index_begins_;
    oa << col_index_ends_;
    oa << col_indexes_;

    new_file.close();
  }
  else {
    LOG_ERROR("Failed to open the index file:" << file_path);
    exit(EXIT_FAILURE);
  }
}

void MassMatch::deserializeMassMatch(std::string new_file, std::string dir_name){
  std::string file_path = dir_name + file_util::getFileSeparator() + new_file;
  std::ifstream file_to_read(file_path, std::ifstream::binary);

  if (file_to_read.is_open()) {

    boost::archive::binary_iarchive ia(file_to_read, std::ios::binary);

    ia >> scale_;
    ia >> proteo_num_;
    ia >> col_num_; 
    ia >> row_num_;

    ia >> proteo_row_begins_;
    ia >> proteo_row_ends_;
    ia >> proteo_minus_water_masses_;
    ia >> row_proteo_ids_;
    ia >> trunc_shifts_;

    ia >> col_index_begins_;
    ia >> col_index_ends_;
    ia >> col_indexes_;

    file_to_read.close();
  } 
  else {
    LOG_ERROR("Failed to open the index file:" << file_path);
    exit(EXIT_FAILURE);
  }
}

void MassMatch::initProteoformBeginEnds(std::vector<std::vector<double>> &shift_2d) {
  // no need to init
  proteo_row_begins_.resize(proteo_num_);
  proteo_row_ends_.resize(proteo_num_);
  int pnt = 0;
  for (int i = 0; i < proteo_num_; i++) {
    proteo_row_begins_[i] = pnt;
    int len = shift_2d[i].size();
    proteo_row_ends_[i] = pnt + len - 1;
    pnt += len;
  }
  row_num_ = pnt;
  row_proteo_ids_.resize(row_num_);
  trunc_shifts_.resize(row_num_);
  for (int i = 0; i < proteo_num_; i++) {
    for (int j = proteo_row_begins_[i]; j <= proteo_row_ends_[i]; j++) {
      row_proteo_ids_[j] = i;
      int pos = j - proteo_row_begins_[i];
      trunc_shifts_[j] = shift_2d[i][pos];
    }
  }
}

inline std::vector<std::vector<int>> convertToInt(std::vector<std::vector<double>> &mass_2d,
                                                  double scale) {
  std::vector<std::vector<int>> result;
  for (size_t i = 0; i < mass_2d.size(); i++) {
    std::vector<int> int_masses;
    for (size_t j = 0; j < mass_2d[i].size(); j++) {
      int value = std::floor(mass_2d[i][j] * scale + 0.5);
      int_masses.push_back(value);
    }
    result.push_back(int_masses);
  }
  return result;
}

void MassMatch::compColumnMatchNums(std::vector<std::vector<int>> &mass_2d,
                                    std::vector<std::vector<int>> &shift_2d,
                                    std::vector<std::vector<int>> &pos_2d,
                                    std::vector<int> &col_match_nums) {
  size_t proteo_num = mass_2d.size();
  for (size_t i = 0; i < proteo_num; i++) {
    for (size_t s = 0; s < shift_2d[i].size(); s++)  {
      for (size_t cur = pos_2d[i][s]; cur < mass_2d[i].size(); cur++) {
        int shift_mass = mass_2d[i][cur] + shift_2d[i][s];
        if (shift_mass > 0) {
          if (shift_mass < col_num_) {
            col_match_nums[shift_mass]++;
          } else {
            break;
          }
        }
      }
    }
  }
}

void MassMatch::fillColumnIndex(std::vector<std::vector<int>> &mass_2d,
                                std::vector<std::vector<int>> &shift_2d,
                                std::vector<std::vector<int>> &pos_2d,
                                std::vector<int> &col_index_pnts) {
  for (size_t i = 0; i < mass_2d.size(); i++) {
    for (size_t s = 0; s < shift_2d[i].size(); s++)  {
      for (size_t cur = pos_2d[i][s]; cur < mass_2d[i].size(); cur++) {
        int shift_mass = mass_2d[i][cur] + shift_2d[i][s];
        if (shift_mass > 0) {
          if (shift_mass < col_num_) {
            col_indexes_[col_index_pnts[shift_mass]] = proteo_row_begins_[i] + s;
            col_index_pnts[shift_mass]++;
          } else {
            break;
          }
        }
      }
    }
  }
}

void MassMatch::initIndexes(std::vector<std::vector<int>> &mass_2d,
                            std::vector<std::vector<double>> &float_shift_2d,
                            std::vector<std::vector<int>> &pos_2d) {
  std::vector<std::vector<int>> shift_2d = convertToInt(float_shift_2d, scale_);
  LOG_DEBUG("column num " << col_num_);
  std::vector<int> col_match_nums(col_num_, 0);
  // no need to initialize
  std::vector<int> col_index_pnts(col_num_);
  col_index_begins_.resize(col_num_);
  col_index_ends_.resize(col_num_);

  compColumnMatchNums(mass_2d, shift_2d, pos_2d, col_match_nums);

  int pnt = 0;
  for (int i = 0; i < col_num_; i++) {
    col_index_begins_[i] = pnt;
    col_index_pnts[i] = pnt;
    col_index_ends_[i] = pnt + col_match_nums[i]-1;
    pnt += col_match_nums[i];
  }
  // no need to initialize
  col_indexes_.resize(pnt, 0);
  LOG_DEBUG("indexes size: "<< pnt);
  fillColumnIndex(mass_2d, shift_2d, pos_2d, col_index_pnts);
}

void MassMatch::compScores(const std::vector<std::pair<int, int>> &pref_mass_errors,
                           std::vector<short> &scores) {
  compScores(pref_mass_errors, 0, 0.0, scores);
}

void MassMatch::compScores(const std::vector<std::pair<int, int>> &pref_mass_errors,
                           int start, double shift, std::vector<short> &scores) {
  int begin_index;
  int end_index;
  int m;
  for (size_t i = start; i < pref_mass_errors.size(); i++) {
    m = pref_mass_errors[i].first + shift;
    // m - errors[i] performs better than m - errors[i] -  errors[bgn_pos]
    int left = m - pref_mass_errors[i].second;
    if (left < 0) {
      left = 0;
    }

    int right = m + pref_mass_errors[i].second;

    if (right < 0 || right >= col_num_) {
      continue;
    }

    begin_index = col_index_begins_[left];
    end_index   = col_index_ends_[right];


    for (int j = begin_index; j <= end_index; j++) {
      scores[col_indexes_[j]]++;
    }
  }
}

void MassMatch::updatePrecScore(const std::pair<int, int> mass_error, 
                                std::vector<short> &scores) {
  // precursor mass
  int begin_index, end_index;
  int m = mass_error.first;
  // m - errors[i] performs better than m - errors[i] -  errors[bgn_pos]
  int left = m - mass_error.second;
  if (left < 0) {
    left = 0;
  }

  int right = m + mass_error.second;
  if (right >= col_num_) {
    right = col_num_ - 1;
  }

  if (left <= right) {
    // update scores
    begin_index = col_index_begins_[left];
    end_index   = col_index_ends_[right];
    for (int j = begin_index; j <= end_index; j++) {
      if (scores[col_indexes_[j]] < getPrecursorMatchScore()) {
        scores[col_indexes_[j]] += getPrecursorMatchScore();
      }
    }
  }
}

void MassMatch::compMatchScores(const std::vector<std::pair<int, int>> &pref_mass_errors,
                                const std::pair<int, int> &prec_minus_water_mass_error,
                                std::vector<short> &scores) {
  compScores(pref_mass_errors, 0, 0.0, scores);
  updatePrecScore(prec_minus_water_mass_error, scores);
}

void MassMatch::compMatchScores(const std::vector<std::pair<int, int>> &pref_mass_errors,
                                const std::vector<std::pair<int, int>> &prec_minus_water_mass_errors,
                                std::vector<short> &scores) {
  compScores(pref_mass_errors, 0, 0.0, scores);
  // precursor mass
  for (size_t i = 0; i < prec_minus_water_mass_errors.size(); i++) {
    std::pair<int, int> mass_error = prec_minus_water_mass_errors[i]; 
    updatePrecScore(mass_error, scores);
  }
}

} /* namespace toppic */
