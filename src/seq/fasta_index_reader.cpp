//Copyright (c) 2014 - 2020, The Trustees of Indiana University.
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

#include <string>
#include <vector>
#include <mutex>

#include "common/util/logger.hpp"
#include "seq/fasta_index_reader.hpp"

namespace toppic {

std::mutex fasta_index_reader_mtx;          

FastaIndexReader::FastaIndexReader(const std::string &file_name) {
  fai_ = fai_load(file_name.c_str());
}

FastaIndexReader::~FastaIndexReader() {
  fai_destroy(fai_);
}

FastaSeqPtr FastaIndexReader::readFastaSeq(const std::string &name,
                                           const std::string &desc) {
  int seq_len;
  // fai_fetch is not multi thread safe
  fasta_index_reader_mtx.lock();          
  char *seq = fai_fetch(fai_, name.c_str(), &seq_len);
  if (seq_len < 0) {
    LOG_WARN("Failed to fetch protein sequence " << name);
  }
  std::string ori_seq(seq);
  fasta_index_reader_mtx.unlock();          

  free(seq);

  return std::make_shared<FastaSeq>(name, desc, ori_seq);
}

}  // namespace toppic
