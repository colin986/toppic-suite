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
#include <iomanip>
#include <map>
#include <string>
#include <vector>
#include <ctime>
#include <limits>

#include "common/base/base_data.hpp"
#include "common/base/mod_util.hpp"
#include "common/base/ptm_util.hpp"
#include "common/util/mem_check.hpp"
#include "common/util/version.hpp"
#include "common/util/console_util.hpp"

#include "seq/fasta_reader.hpp"
#include "seq/fasta_util.hpp"

#include "ms/spec/msalign_util.hpp"
#include "ms/spec/msalign_frac_merge.hpp"
#include "ms/spec/deconv_json_merge.hpp"
#include "ms/env/env_base.hpp"
#include "ms/feature/feature_merge.hpp"

#include "para/prsm_para.hpp"
#include "prsm/prsm_str_merge.hpp"
#include "prsm/prsm_top_selector.hpp"
#include "prsm/prsm_simple_cluster.hpp"
#include "prsm/prsm_feature_cluster.hpp"
#include "prsm/prsm_cutoff_selector.hpp"
#include "prsm/prsm_match_table_writer.hpp"
#include "prsm/prsm_fdr.hpp"
#include "prsm/prsm_fdr_groups.hpp"
#include "prsm/prsm_form_filter.hpp"
#include "prsm/prsm_util.hpp"
#include "prsm/prsm_coverage.hpp"

#include "filter/mng/zero_ptm_filter_mng.hpp"
#include "filter/mng/index_file_name.hpp"
#include "filter/zeroptm/zero_ptm_filter_processor.hpp"
#include "search/zeroptmsearch/zero_ptm_search_mng.hpp"
#include "search/zeroptmsearch/zero_ptm_search_processor.hpp"

#include "filter/mng/var_ptm_filter_mng.hpp"
#include "filter/varptm/var_ptm_filter_processor.hpp"
#include "search/varptmsearch/var_ptm_search_mng.hpp"
#include "search/varptmsearch/var_ptm_search_processor.hpp"

#include "filter/mng/one_ptm_filter_mng.hpp"
#include "filter/oneptm/one_ptm_filter_processor.hpp"
#include "search/oneptmsearch/ptm_search_mng.hpp"
#include "search/oneptmsearch/one_ptm_search_processor.hpp"

#include "filter/mng/diag_filter_mng.hpp"
#include "filter/diag/diag_filter_processor.hpp"
#include "search/ptmsearch/ptm_search_processor.hpp"

#include "stat/tdgf/tdgf_mng.hpp"
#include "stat/tdgf/evalue_processor.hpp"

#include "stat/local/local_mng.hpp"
#include "stat/local/local_processor.hpp"

#include "visual/xml_generator.hpp"
#include "visual/json_transformer.hpp"

#include "console/toppic_argument.hpp"

namespace toppic {

void cleanToppicDir(const std::string &fa_name, 
                    const std::string &sp_name,
                    bool keep_temp_files) {
  std::string abs_sp_name = file_util::absoluteName(sp_name);
  std::string sp_base = file_util::basename(abs_sp_name);
  std::replace(sp_base.begin(), sp_base.end(), '\\', '/');
  file_util::delFile(sp_base + "_toppic_proteoform.xml");
  bool overwrite = true;
  file_util::copyFile(sp_base + ".toppic_form_cutoff_form",
                      sp_base + "_toppic_proteoform.xml", overwrite);
  file_util::delFile(sp_base + "_toppic_prsm.xml");
  file_util::copyFile(sp_base + "." + "toppic_prsm_cutoff",
                      sp_base + "_toppic_prsm.xml", overwrite);
  if (!keep_temp_files) {
    file_util::cleanPrefix(sp_name, sp_base + ".msalign_");
    file_util::delFile(abs_sp_name + "_index");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_zero_filter_");
    file_util::delFile(sp_base + ".toppic_zero_shift");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_zero_shift_");

    file_util::cleanPrefix(sp_name, sp_base + ".toppic_var_filter_");
    file_util::delFile(sp_base + ".toppic_var_ptm");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_var_ptm_");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_one_filter_");
    file_util::delFile(sp_base + ".toppic_one_shift");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_one_shift_");
    file_util::delFile(sp_base + ".toppic_multi_filter");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_multi_filter_");
    file_util::delFile(sp_base + ".toppic_multi_shift");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_multi_shift_");
    file_util::delFile(sp_base + ".toppic_combined");
    file_util::delFile(sp_base + ".toppic_evalue");
    file_util::cleanPrefix(sp_name, sp_base + ".toppic_evalue_");
    file_util::delFile(sp_base + ".toppic_cluster");
    file_util::delFile(sp_base + ".toppic_cluster_fdr");
    file_util::delFile(sp_base + ".toppic_cluster_local");
    file_util::delFile(sp_base + ".toppic_prsm_cutoff");
    file_util::delFile(sp_base + ".toppic_form_cutoff");
    file_util::delFile(sp_base + ".toppic_form_cutoff_form");
    file_util::delDir(sp_base + "_toppic_proteoform_cutoff_xml");
    file_util::delDir(sp_base + "_toppic_prsm_cutoff_xml");
  }
}

// Test modification files. 
int TopPIC_testModFile(std::map<std::string, std::string> & arguments) {
  try {
    base_data::init();
    LOG_DEBUG("Init base data completed");

    // Test arguments
    PrsmParaPtr prsm_para_ptr = std::make_shared<PrsmPara>(arguments);

    if (arguments["variablePtmFileName"] != "") {
      mod_util::readModTxt(arguments["variablePtmFileName"]);
      ptm_util::readPtmTxt(arguments["variablePtmFileName"]);
    }

    if (arguments["localPtmFileName"] != "") {
      mod_util::readModTxt(arguments["localPtmFileName"]);
      ptm_util::readPtmTxt(arguments["localPtmFileName"]);
    }
  } catch (const char* e) {
    std::cout << "[Exception]" << std::endl;
    std::cout << e << std::endl;
    exit(EXIT_FAILURE);
  }
  return 0;
}

// protein filtering + database searching + E-value computation
int TopPIC_identify(std::map<std::string, std::string> & arguments) {
  try {
    std::time_t start = time(nullptr);
    char buf[50];
    std::strftime(buf, 50, "%a %b %d %H:%M:%S %Y", std::localtime(&start));

    arguments["startTime"] = buf;
    ToppicArgument::outputArguments(std::cout, " ", arguments);

    std::string resource_dir = arguments["resourceDir"];
    base_data::init();
    EnvBase::initBase(resource_dir);

    LOG_DEBUG("Init base data completed");

    std::string db_file_name = arguments["databaseFileName"];
    std::string sp_file_name = arguments["spectrumFileName"];
    std::string ori_db_file_name = arguments["oriDatabaseFileName"];
    std::string feature_file_name = file_util::basename(sp_file_name) + ".feature";

    if (arguments["useFeatureFile"] == "true") {
      if (!file_util::exists(feature_file_name)) {
        LOG_ERROR("TopFD feature file does not exist!.");
        LOG_ERROR("Please use -x option in command line or select 'Missing MS1 feature file in GUI'.");
        return 1;
      }
    }

    int n_top = std::stoi(arguments["numOfTopPrsms"]);
    int var_ptm_num = std::stoi(arguments["variablePtmNum"]);
    int shift_num = std::stoi(arguments["shiftNumber"]);
    std::string var_ptm_file_name = arguments["variablePtmFileName"]; 

    double max_shift_mass = std::stod(arguments["maxShiftMass"]);
    double min_shift_mass = std::stod(arguments["minShiftMass"]);

    int filter_result_num = std::stoi(arguments["filteringResultNumber"]);
    int thread_num = std::stoi(arguments["threadNumber"]);

    // Filter steps requires a large amount of memory. 
    // We use only one thread to reduce the memory requirement.
    int filter_thread_num = mem_check::getMaxThreads("zero_one_shift_filter");
    if (filter_thread_num > thread_num) {
      filter_thread_num = thread_num;
    }
    LOG_DEBUG("Filter thread number " << filter_thread_num);
    int diag_filter_thread_num = mem_check::getMaxThreads("diag_filter");
    if (diag_filter_thread_num > thread_num) {
      diag_filter_thread_num = thread_num;
    }

    bool use_gf = true;
    if (arguments["useLookupTable"] == "true") {
      use_gf = false;
    }

    // var_ptm_type number is used for E-value computation
    int var_ptm_type_num = 0;

    // index file name
    IndexFileNamePtr file_name_ptr = std::make_shared<IndexFileName>();
    std::string index_file_para = file_name_ptr->geneFileName(arguments);

    PrsmParaPtr prsm_para_ptr = std::make_shared<PrsmPara>(arguments);
    LOG_DEBUG("prsm para inited");

    bool decoy = false;
    if (arguments["searchType"] == "TARGET+DECOY") {
      decoy = true;
    }
    LOG_DEBUG("block size " << arguments["databaseBlockSize"]);
    int db_block_size = std::stoi(arguments["databaseBlockSize"]);
    int max_frag_len = std::stoi(arguments["maxFragmentLength"]);
    int min_block_num = std::stoi(arguments["minBlockNum"]);
    fasta_util::dbPreprocess(ori_db_file_name, db_file_name, decoy, 
                             db_block_size, max_frag_len, min_block_num);
    msalign_util::geneSpIndex(sp_file_name);

    std::vector<std::string> input_exts;
    
    std::cout << "Zero unexpected shift filtering - started." << std::endl;
    ZeroPtmFilterMngPtr zero_filter_mng_ptr
        = std::make_shared<ZeroPtmFilterMng>(prsm_para_ptr, index_file_para, 
                                             filter_thread_num, "toppic_zero_filter");
    zero_ptm_filter_processor::process(zero_filter_mng_ptr);
    std::cout << "Zero unexpected shift filtering - finished." << std::endl;

    std::cout << "Zero unexpected shift search - started." << std::endl;
    ZeroPtmSearchMngPtr zero_search_mng_ptr
        = std::make_shared<ZeroPtmSearchMng>(prsm_para_ptr, "toppic_zero_filter", "toppic_zero_shift");
    ZeroPtmSearchProcessorPtr zero_search_processor
        = std::make_shared<ZeroPtmSearchProcessor>(zero_search_mng_ptr);
    zero_search_processor->process();
    zero_search_processor = nullptr;
    std::cout << "Zero unexpected shift search - finished." << std::endl;
    
    input_exts.push_back("toppic_zero_shift_COMPLETE");
    input_exts.push_back("toppic_zero_shift_PREFIX");
    input_exts.push_back("toppic_zero_shift_SUFFIX");
    input_exts.push_back("toppic_zero_shift_INTERNAL");

    if (var_ptm_num >= 1 && var_ptm_file_name != "") {
      std::cout << "Variable PTM filtering - started." << std::endl;
      bool use_approx_spec = false;
      if (arguments["useApproxSpectra"] =="true") {
        use_approx_spec = true;
      }
      VarPtmFilterMngPtr var_filter_mng_ptr
        = std::make_shared<VarPtmFilterMng>(prsm_para_ptr, index_file_para, 
                                            var_ptm_file_name, var_ptm_num, 
                                            filter_thread_num, use_approx_spec, 
                                            "toppic_var_filter");
      var_ptm_type_num = var_filter_mng_ptr->getSingleShiftNum(); 
      var_ptm_filter_processor::process(var_filter_mng_ptr);
      std::cout << "Variable PTM filtering - finished." << std::endl;

      std::cout << "Variable PTM search - started." << std::endl;
      VarPtmSearchMngPtr var_ptm_search_mng_ptr
        = std::make_shared<VarPtmSearchMng>(prsm_para_ptr, n_top, var_ptm_file_name, 
                                            var_ptm_num, thread_num, "toppic_var_filter", "toppic_var_ptm");
      VarPtmSearchProcessorPtr var_ptm_search_processor
          = std::make_shared<VarPtmSearchProcessor>(var_ptm_search_mng_ptr);
      var_ptm_search_processor->process();
      var_ptm_search_processor = nullptr;
      std::cout << "Variable PTM search - finished." << std::endl;
      input_exts.push_back("toppic_var_ptm_COMPLETE");
      input_exts.push_back("toppic_var_ptm_PREFIX");
      input_exts.push_back("toppic_var_ptm_SUFFIX");
      input_exts.push_back("toppic_var_ptm_INTERNAL");
    }

    if (shift_num >= 1) {
      std::cout << "One unexpected shift filtering - started." << std::endl;
      OnePtmFilterMngPtr one_ptm_filter_mng_ptr
          = std::make_shared<OnePtmFilterMng>(prsm_para_ptr, index_file_para, 
                                              "toppic_one_filter", 
                                              filter_thread_num, 
                                              min_shift_mass, max_shift_mass); 
      one_ptm_filter_processor::process(one_ptm_filter_mng_ptr);
      std::cout << "One unexpected shift filtering - finished." << std::endl;

      std::cout << "One unexpected shift search - started." << std::endl;
      int search_shift_num = 1;
      PtmSearchMngPtr one_search_mng_ptr
          = std::make_shared<PtmSearchMng>(prsm_para_ptr, n_top, min_shift_mass, max_shift_mass,
                                           search_shift_num, thread_num, "toppic_one_filter", "toppic_one_shift");
      OnePtmSearchProcessorPtr one_search_processor
          = std::make_shared<OnePtmSearchProcessor>(one_search_mng_ptr);
      one_search_processor->process();
      one_search_processor = nullptr;
      std::cout << "One unexpected shift search - finished." << std::endl;

      input_exts.push_back("toppic_one_shift_COMPLETE");
      input_exts.push_back("toppic_one_shift_PREFIX");
      input_exts.push_back("toppic_one_shift_SUFFIX");
      input_exts.push_back("toppic_one_shift_INTERNAL");
    }

    if (shift_num >= 2) {
      std::cout << "Multiple unexpected shifts filtering - started." << std::endl;
      // thread number is used because diagonal filter uses only one index
      DiagFilterMngPtr diag_filter_mng_ptr
          = std::make_shared<DiagFilterMng>(prsm_para_ptr, index_file_para,  
                                            filter_result_num, diag_filter_thread_num, 
                                            "toppic_multi_filter");
      DiagFilterProcessorPtr diag_filter_processor
          = std::make_shared<DiagFilterProcessor>(diag_filter_mng_ptr);
      diag_filter_processor->process();
      diag_filter_processor = nullptr;
      std::cout << "Multiple unexpected shifts filtering - finished." << std::endl;

      std::cout << "Multiple unexpected shifts search - started." << std::endl;
      PtmSearchMngPtr multi_search_mng_ptr
          = std::make_shared<PtmSearchMng>(prsm_para_ptr, n_top, min_shift_mass, max_shift_mass,
                                           shift_num, thread_num, "toppic_multi_filter", "toppic_multi_shift");
      PtmSearchProcessorPtr processor = std::make_shared<PtmSearchProcessor>(multi_search_mng_ptr);
      processor->process();
      processor = nullptr;
      std::cout << "Multiple unexpected shifts search - finished." << std::endl;

      input_exts.push_back("toppic_multi_shift_COMPLETE_2");
      input_exts.push_back("toppic_multi_shift_PREFIX_2");
      input_exts.push_back("toppic_multi_shift_SUFFIX_2");
      input_exts.push_back("toppic_multi_shift_INTERNAL_2");
    }

    std::cout << "Merging PrSMs - started." << std::endl;
    int prsm_top_num = std::numeric_limits<int>::max();
    bool norm = false;
    bool remove_dup = false;
    PrsmStrMergePtr merge_ptr
        = std::make_shared<PrsmStrMerge>(sp_file_name, input_exts, "toppic_combined", 
                                         prsm_top_num, norm, remove_dup);
    merge_ptr->process();
    merge_ptr = nullptr;
    std::cout << "Merging PrSMs - finished." << std::endl;
    
    std::cout << "E-value computation - started." << std::endl;
    TdgfMngPtr tdgf_mng_ptr
        = std::make_shared<TdgfMng>(prsm_para_ptr, shift_num,
                                    std::max(std::abs(max_shift_mass), std::abs(min_shift_mass)),
                                    use_gf, var_ptm_type_num, thread_num, 
                                    "toppic_combined", "toppic_evalue");
    EValueProcessorPtr processor = std::make_shared<EValueProcessor>(tdgf_mng_ptr);
    processor->init();
    // compute E-value for a set of prsms each run
    processor->process(false);
    processor = nullptr;
    std::cout << "E-value computation - finished." << std::endl;

    std::cout << "Top PrSM selecting - started" << std::endl;
    prsm_top_selector::process(sp_file_name, "toppic_evalue", "toppic_raw_prsm", n_top);
    std::cout << "Top PrSM selecting - finished." << std::endl;

  } catch (const char* e) {
    std::cout << "[Exception]" << std::endl;
    std::cout << e << std::endl;
  }
  return 0;
}

// proteoform clustering + FDR + HTML generation
int TopPIC_post(std::map<std::string, std::string> & arguments) {
  try {
    std::string resource_dir = arguments["resourceDir"];

    base_data::init();
    LOG_DEBUG("Init base data completed");

    std::string sp_file_name = arguments["spectrumFileName"];
    std::string ori_db_file_name = arguments["oriDatabaseFileName"];
    std::string db_file_name = ori_db_file_name + "_idx" + file_util::getFileSeparator() 
      + file_util::filenameFromEntirePath(arguments["databaseFileName"]);    
    double max_shift_mass = std::stod(arguments["maxShiftMass"]);
    double min_shift_mass = std::stod(arguments["minShiftMass"]);

    bool localization = false;
    if (arguments["localPtmFileName"] != "") {
      localization = true;
    }

    PrsmParaPtr prsm_para_ptr = std::make_shared<PrsmPara>(arguments);
    msalign_util::geneSpIndex(sp_file_name);
    LOG_DEBUG("prsm para inited");

    std::cout << "Finding PrSM clusters - started." << std::endl;
    bool is_proteoform_ppm_error = (arguments["proteoformPpmError"] == "true"); 
    double proteoform_error_tole = std::stod(arguments["proteoformErrorTolerance"]);
    if (is_proteoform_ppm_error) {
      proteoform_error_tole = proteoform_error_tole/1000000;
    }
    LOG_DEBUG("form error tole " << proteoform_error_tole);

    if (arguments["useFeatureFile"] == "true") {
      // TopFD msalign file with feature ID
      ModPtrVec fix_mod_list = prsm_para_ptr->getFixModPtrVec();
      prsm_feature_cluster::process(sp_file_name,
                                    "toppic_raw_prsm",
                                    "toppic_cluster",
                                    is_proteoform_ppm_error,
                                    proteoform_error_tole);
    } 
    else {
      prsm_simple_cluster::process(db_file_name, sp_file_name,
                                   "toppic_raw_prsm", prsm_para_ptr->getFixModPtrVec(),
                                   "toppic_cluster", is_proteoform_ppm_error, 
                                   proteoform_error_tole);
    }
    std::cout << "Finding PrSM clusters - finished." << std::endl;
    std::string cur_suffix = "toppic_cluster";

    if (localization) {
      std::cout << "PTM characterization - started." << std::endl;
      LocalMngPtr local_mng
          = std::make_shared<LocalMng>(prsm_para_ptr,
                                       std::stod(arguments["localThreshold"]),
                                       arguments["localPtmFileName"],
                                       min_shift_mass,
                                       max_shift_mass,
                                       cur_suffix,  
                                       "toppic_cluster_local");
      LocalProcessorPtr local_ptr = std::make_shared<LocalProcessor>(local_mng);
      local_ptr->process();
      local_ptr = nullptr;
      std::cout << "PTM characterization - finished." << std::endl;
      cur_suffix = "toppic_cluster_local";
    }

    std::time_t end = time(nullptr);
    char buf[50];
    std::strftime(buf, 50, "%a %b %d %H:%M:%S %Y", std::localtime(&end));
    arguments["endTime"] = buf;

    std::string argu_str = ToppicArgument::outputTsvArguments(arguments);

    if (arguments["outputRawPrsmTable"] == "true"){
      std::cout << "Outputting Raw PrSM table - started." << std::endl;
      PrsmMatchTableWriterPtr raw_table_out
        = std::make_shared<PrsmMatchTableWriter>(prsm_para_ptr, argu_str, cur_suffix);
      raw_table_out->write("_toppic_raw_prsm_single.tsv", false);
      raw_table_out->write("_toppic_raw_prsm.tsv", true);

      raw_table_out = nullptr;
      std::cout << "Outputting Raw PrSM table - finished." << std::endl;
    }

    if (arguments["searchType"] == "TARGET+DECOY") {
      std::cout << "FDR computation - started. " << std::endl;
      prsm_fdr::process(sp_file_name, "toppic_cluster", "toppic_cluster_fdr", arguments["keepDecoyResults"]);
      //prsm_fdr_groups::process(sp_file_name, "toppic_cluster", "toppic_cluster_fdr", arguments["keepDecoyResults"]);
      std::cout << "FDR computation - finished." << std::endl;
      cur_suffix = "toppic_cluster_fdr";
    }


    std::string cutoff_type = arguments["cutoffSpectralType"];
    std::cout << "PrSM filtering by " << cutoff_type << " - started." << std::endl;
    double cutoff_value;
    std::istringstream(arguments["cutoffSpectralValue"]) >> cutoff_value;
    prsm_cutoff_selector::process(db_file_name, sp_file_name, cur_suffix,
                                  "toppic_prsm_cutoff", cutoff_type, cutoff_value);
    std::cout << "PrSM filtering by " << cutoff_type << " - finished." << std::endl;
    cur_suffix = "toppic_prsm_cutoff";

    std::cout << "Outputting PrSM table - started." << std::endl;
    PrsmMatchTableWriterPtr table_out
        = std::make_shared<PrsmMatchTableWriter>(prsm_para_ptr, argu_str, 
                                                 cur_suffix);
    table_out->write("_toppic_prsm_single.tsv", false);
    table_out->write("_toppic_prsm.tsv", true);

    table_out = nullptr;
    std::cout << "Outputting PrSM table - finished." << std::endl;

    XmlGeneratorPtr xml_gene = std::make_shared<XmlGenerator>(prsm_para_ptr, resource_dir, 
                                                              cur_suffix, "toppic_prsm_cutoff");
    if (arguments["geneHTMLFolder"] == "true"){
      std::cout << "Generating PrSM XML files - started." << std::endl;
    
      xml_gene->process();
      xml_gene = nullptr;
      std::cout << "Generating PrSM XML files - finished." << std::endl;

      console_util::copyTopMSV(arguments);
  
      std::cout << "Converting PrSM XML files to JSON files - started." << std::endl;
      jsonTranslate(arguments, "toppic_prsm_cutoff");
      std::cout << "Converting PrSM XML files to JSON files - finished." << std::endl;
    }

    if (arguments["outputPrsmCoverage"] == "true") {
      std::cout << "Outputting PrSM coverage - started." << std::endl;
      PrsmCoveragePtr cov_ptr = std::make_shared<PrsmCoverage>(prsm_para_ptr, "toppic_prsm_cutoff", "toppic_prsm_coverage");
      cov_ptr->processSingleCoverage();
      cov_ptr = nullptr;
      std::cout << "Outputting PrSM coverage - finished." << std::endl;
    }

    cutoff_type = (arguments["cutoffProteoformType"] == "FDR") ? "FORMFDR": "EVALUE";
    std::cout << "PrSM filtering by " << cutoff_type << " - started." << std::endl;
    std::istringstream(arguments["cutoffProteoformValue"]) >> cutoff_value;
    prsm_cutoff_selector::process(db_file_name, sp_file_name, cur_suffix,
                                  "toppic_form_cutoff", cutoff_type,
                                  cutoff_value);
    std::cout << "PrSM filtering by " << cutoff_type << " - finished." << std::endl;

    std::cout << "Selecting top PrSMs for proteoforms - started." << std::endl;
    prsm_form_filter::process(db_file_name, sp_file_name, "toppic_form_cutoff",
                              "toppic_form_cutoff_form");
    std::cout << "Selecting top PrSMs for proteoforms - finished." << std::endl;
    std::cout << "Outputting proteoform table - started." << std::endl;
    PrsmMatchTableWriterPtr form_out
      = std::make_shared<PrsmMatchTableWriter>(prsm_para_ptr, argu_str, "toppic_form_cutoff_form");  
    form_out->write("_toppic_proteoform_single.tsv", false);
    form_out->write("_toppic_proteoform.tsv", true);
    form_out = nullptr;
    std::cout << "Outputting proteoform table - finished." << std::endl;

    if (arguments["geneHTMLFolder"] == "true"){

      std::cout << "Generating proteoform XML files - started." << std::endl;
      xml_gene = std::make_shared<XmlGenerator>(prsm_para_ptr, resource_dir, 
                                              "toppic_form_cutoff", 
                                              "toppic_proteoform_cutoff");
    
      xml_gene->process();
      xml_gene = nullptr;
      std::cout << "Generating proteoform XML files - finished." << std::endl;
      std::cout << "Converting proteoform XML files to HTML files - started." << std::endl;
      jsonTranslate(arguments, "toppic_proteoform_cutoff");
      std::cout << "Converting proteoform XML files to HTML files - finished." << std::endl;
    }
  } catch (const char* e) {
    std::cout << "[Exception]" << std::endl;
    std::cout << e << std::endl;
  }
  return 0;
}

int TopPICProgress(std::map<std::string, std::string> & arguments) {
  TopPIC_identify(arguments); 
  TopPIC_post(arguments);
  return 0;
}

int TopPICProgress_multi_file(std::map<std::string, std::string> & arguments,
                              const std::vector<std::string> & spec_file_lst) {

  std::string base_path = file_util::absoluteDir(spec_file_lst[0]);

  std::time_t start = time(nullptr);
  char buf[50];
  std::strftime(buf, 50, "%a %b %d %H:%M:%S %Y", std::localtime(&start));
  std::string combined_start_time = buf;

  std::cout << "TopPIC " << toppic::Version::getVersion() << std::endl;
  arguments["version"] = toppic::Version::getVersion();

  xercesc::XMLPlatformUtils::Initialize(); 

  TopPIC_testModFile(arguments);

  //check if a combined file name given in -c parameter is the same as 
  //one of the input spectrum file. If so, throw error.
  if (arguments["combinedOutputName"] != "") {
    std::string merged_file_name = arguments["combinedOutputName"] + "_ms2.msalign"; 
    for (size_t k = 0; k < spec_file_lst.size(); k++) {
      if (merged_file_name == spec_file_lst[k]) {
        std::string raw_file_name = spec_file_lst[k].substr(0, spec_file_lst[k].find("_ms2.msalign"));
        LOG_ERROR("A combined file name cannot be the same as one of the input file names '" 
                  << raw_file_name << "'. Please choose a different name for a combined file and retry.");
        return 1;
      }
    }
  }

  bool keep_temp_files = (arguments["keepTempFiles"] == "true"); 
  std::string ori_db_file_name = arguments["oriDatabaseFileName"];

  bool combine_result_only = (arguments["combineResultOnly"] == "true"); 

  if (!combine_result_only) {
    for (size_t k = 0; k < spec_file_lst.size(); k++) {
      std::strftime(buf, 50, "%a %b %d %H:%M:%S %Y", std::localtime(&start));
      std::string start_time = buf;
      arguments["startTime"] = start_time;
      arguments["spectrumFileName"] = spec_file_lst[k];
      if (toppic::TopPICProgress(arguments) != 0) {
        return 1;
      }
      cleanToppicDir(ori_db_file_name, spec_file_lst[k], keep_temp_files);
    }
  }

  if (arguments["combinedOutputName"] != "") {
    std::string merged_file_name = arguments["combinedOutputName"]; 
    std::vector<std::string> raw_file_list;
    for (size_t k = 0; k < spec_file_lst.size(); k++) {
      std::string raw_file_name = spec_file_lst[k].substr(0, spec_file_lst[k].find("_ms2.msalign"));
      raw_file_list.push_back(raw_file_name);
    }
    std::string para_str = "";
    std::cout << "Merging files started." << std::endl;
    std::cout << "Merging msalign files started." << std::endl;
    msalign_frac_merge::mergeFractions(raw_file_list, merged_file_name, para_str); 
    std::cout << "Merging msalign files finished." << std::endl;

    if (arguments["useFeatureFile"] == "true") {//only when feature files are being used
      std::cout << "Merging feature files started." << std::endl;
      feature_merge::process(raw_file_list, merged_file_name, para_str);
      std::cout << "Merging feature files finished." << std::endl;
    }
    // merge TOP files
    std::cout << "Merging identification files started." << std::endl;
    std::vector<std::string> prsm_file_lst(spec_file_lst.size());
    for (size_t i = 0; i < spec_file_lst.size(); i++) {
      prsm_file_lst[i] = file_util::basename(spec_file_lst[i]) + ".toppic_raw_prsm"; 
    }
    prsm_util::mergePrsmFiles(prsm_file_lst, SpPara::getMaxSpecNumPerFile(), 
                              SpPara::getMaxFeatureNumPerFile(),
                              merged_file_name + "_ms2.toppic_raw_prsm");
    std::cout << "Merging identification files finished." << std::endl;
    std::cout << "Merging files - finished." << std::endl;

    std::string sp_file_name = merged_file_name + "_ms2.msalign";
    arguments["spectrumFileName"] = sp_file_name;
    arguments["startTime"] = combined_start_time;
    // do not generate html files for combined file
    arguments["geneHTMLFolder"] = "false";

    TopPIC_post(arguments);
    sp_file_name = merged_file_name + "_ms2.msalign";
    cleanToppicDir(ori_db_file_name, sp_file_name, keep_temp_files);
  }

  base_data::release();

  std::cout << "TopPIC finished." << std::endl << std::flush;

  return 0;
}

}  // namespace toppic
