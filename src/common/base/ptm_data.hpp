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
#ifndef TOPPIC_COMMON_BASE_PTM_DATA_HPP_
#define TOPPIC_COMMON_BASE_PTM_DATA_HPP_

#include <string>

namespace toppic {

std::string ptm_base_data = R"(
<ptm_list>
<ptm>
  <name>No PTM</name>
  <abbreviation>No PTM</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>0</mono_mass>
</ptm>
<ptm>
  <name>Phosphorylation</name>
  <abbreviation>Phospho</abbreviation>
  <unimod>21</unimod>
  <mono_mass>79.966331</mono_mass>
</ptm> 
<ptm>
  <name>Acetylation</name>
  <abbreviation>Acetyl</abbreviation>
  <unimod>1</unimod>
  <mono_mass>42.010565</mono_mass>
</ptm>
<ptm>
  <name>Oxidation</name>
  <abbreviation>Oxidation</abbreviation>
  <unimod>35</unimod>
  <mono_mass>15.994915</mono_mass>
</ptm>
<ptm>
  <name>Dehydration</name>
  <abbreviation>Dehydrated</abbreviation>
  <unimod>23</unimod>
  <mono_mass>-18.010565</mono_mass>
</ptm>
<ptm>
  <name>Methylation</name>
  <abbreviation>Methyl</abbreviation>
  <unimod>34</unimod>
  <mono_mass>14.015650</mono_mass>
</ptm>
<ptm>
  <name>Dioxidation</name>
  <abbreviation>Dioxidation</abbreviation>
  <unimod>425</unimod>
  <mono_mass>31.989829</mono_mass>
</ptm>
<ptm>
  <name>Cysteinylation</name>
  <abbreviation>Cysteinyl</abbreviation>
  <unimod>312</unimod>
  <mono_mass>119.004099</mono_mass>
</ptm>
<ptm>
  <name>Biotinylation</name>
  <abbreviation>Biotin</abbreviation>
  <unimod>3</unimod>
  <mono_mass>226.077598</mono_mass>
</ptm>
<ptm>
  <name>di-Methylation</name>
  <abbreviation>Dimethyl</abbreviation>
  <unimod>36</unimod>
  <mono_mass>28.031300</mono_mass>
</ptm>
<ptm>
  <name>tri-Methylation</name>
  <abbreviation>Trimethyl</abbreviation>
  <unimod>37</unimod>
  <mono_mass>42.046950</mono_mass>
</ptm>
<ptm>
  <name>Gamma-carboxyglutamic acid</name>
  <abbreviation>carboxylation</abbreviation>
  <unimod>299</unimod>
  <mono_mass>43.989829</mono_mass>
</ptm>
<ptm>
  <name>Carbamidomethylation</name>
  <abbreviation>Carbamidomethylation</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>57.021464</mono_mass>
</ptm>
<ptm>
  <name>Iodoacetic acid derivative</name>
  <abbreviation>Carboxymethyl</abbreviation>
  <unimod>6</unimod>
  <mono_mass>58.005479</mono_mass>
</ptm>
<ptm>
  <name>ADP Ribose addition</name>
  <abbreviation>ADP-Ribosyl</abbreviation>
  <unimod>213</unimod>
  <mono_mass>541.061110</mono_mass>
</ptm>
<ptm>
  <name>Succinic anhydride labeling reagent light form (N-term &amp; K)</name>
  <abbreviation>Succinyl</abbreviation>
  <unimod>64</unimod>
  <mono_mass>100.016044</mono_mass>
</ptm>
<ptm>
  <name>S-nitrosylation</name>
  <abbreviation>Nitrosyl</abbreviation>
  <unimod>275</unimod>
  <mono_mass>28.990164</mono_mass>
</ptm>
<ptm>
  <name>Oxoalanine</name>
  <abbreviation>Oxoalanine</abbreviation>
  <unimod>402</unimod>
  <mono_mass>-17.992806</mono_mass>
</ptm>
<ptm>
  <name>carboxyethyl</name>
  <abbreviation>Carboxyethyl</abbreviation>
  <unimod>378</unimod>
  <mono_mass>72.021129</mono_mass>
</ptm>
<ptm>
  <name>Carboxylation</name>
  <abbreviation>Carboxy</abbreviation>
  <unimod>299</unimod>
  <mono_mass>43.989829</mono_mass>
</ptm>
<ptm>
  <name>Decarboxylation</name>
  <abbreviation>Decarboxy</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>-43.989829</mono_mass>
</ptm>
<ptm>
  <name>Deamidation</name>
  <abbreviation>Deamidated</abbreviation>
  <unimod>7</unimod>
  <mono_mass>0.984016</mono_mass>
</ptm>
<ptm>
  <name>Amidation</name>
  <abbreviation>Amidated</abbreviation>
  <unimod>2</unimod>
  <mono_mass>-0.984016</mono_mass>
</ptm>
<ptm>
  <name>Malonylation</name>
  <abbreviation>Malonyl</abbreviation>
  <unimod>747</unimod>
  <mono_mass>86.000394</mono_mass>
</ptm>
<ptm>
  <name>Lipoyl</name>
  <abbreviation>Lipoyl</abbreviation>
  <unimod>42</unimod>
  <mono_mass>188.032956</mono_mass>
</ptm>
<ptm>
  <name>Allysine</name>
  <abbreviation>Allysine</abbreviation>
  <unimod>352</unimod>
  <mono_mass>-1.031634</mono_mass>
</ptm>
<ptm>
  <name>O-Sulfonation</name>
  <abbreviation>Sulfo</abbreviation>
  <unimod>40</unimod>
  <mono_mass>79.956815</mono_mass>
</ptm>
<ptm>
  <name>Pyridoxal phosphate</name>
  <abbreviation>PyridoxalPhosphate</abbreviation>
  <unimod>46</unimod>
  <mono_mass>229.014009</mono_mass>
</ptm>
<ptm>
  <name>Citrullination</name>
  <abbreviation>Citrullination</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>0.984016</mono_mass>
</ptm>
<ptm>
  <name>Diphthamide</name>
  <abbreviation>Diphthamide</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>143.117890</mono_mass>
</ptm>
<ptm>
  <name>AMP</name>
  <abbreviation>Phosphoadenosine</abbreviation>
  <unimod>405</unimod>
  <mono_mass>329.052520</mono_mass>
</ptm>
<ptm>
  <name>Crotonylation</name>
  <abbreviation>Crotonyl</abbreviation>
  <unimod>1363</unimod>
  <mono_mass>68.026215</mono_mass>
</ptm>
<ptm>
  <name>Retinal</name>
  <abbreviation>Retinylidene</abbreviation>
  <unimod>380</unimod>
  <mono_mass>266.203451</mono_mass>
</ptm>
<ptm>
  <name>Flavin adenine dinucleotide</name>
  <abbreviation>FAD</abbreviation>
  <unimod>50</unimod>
  <mono_mass>783.141486</mono_mass>
</ptm>
<ptm>
  <name>glutathione disulfide</name>
  <abbreviation>Glutathione</abbreviation>
  <unimod>55</unimod>
  <mono_mass>305.068156</mono_mass>
</ptm>
<ptm>
  <name>Nitration</name>
  <abbreviation>Nitro</abbreviation>
  <unimod>354</unimod>
  <mono_mass>44.985078</mono_mass>
</ptm>
<ptm>
  <name>Topaquinone</name>
  <abbreviation>TPQ</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>29.974179</mono_mass>
</ptm>
<ptm>
  <name>tetraiodo</name>
  <abbreviation>Thyroxine</abbreviation>
  <unimod>398</unimod>
  <mono_mass>595.612807</mono_mass>
</ptm>
<ptm>
  <name>triiodo</name>
  <abbreviation>Triiodothyronine</abbreviation>
  <unimod>397</unimod>
  <mono_mass>469.716159</mono_mass>
</ptm>
<ptm>
  <name>Cholinephosphoryl</name>
  <abbreviation>Cholinephosphoryl</abbreviation>
  <unimod>-1</unimod>
  <mono_mass>166.062756</mono_mass>
</ptm>
<ptm>
  <name>Phosphopantetheine</name>
  <abbreviation>Phosphopantetheine</abbreviation>
  <unimod>49</unimod>
  <mono_mass>340.085794</mono_mass>
</ptm>
<ptm>
  <name>hypusine</name>
  <abbreviation>Hypusine</abbreviation>
  <unimod>379</unimod>
  <mono_mass>87.068414</mono_mass>
</ptm>
<ptm>
  <name>Pyro-glu from E</name>
  <abbreviation>Glu-&gt;pyro-Glu</abbreviation>
  <unimod>27</unimod>
  <mono_mass>-18.010565</mono_mass>
</ptm>
<ptm>
  <name>Thiocarboxylic acid</name>
  <abbreviation>Thiocarboxy</abbreviation>
  <unimod>420</unimod>
  <mono_mass>15.977156</mono_mass>
</ptm>
<ptm>
  <name>Pyro-glu from Q</name>
  <abbreviation>Gln-&gt;pyro-Glu</abbreviation>
  <unimod>28</unimod>
  <mono_mass>-17.026549</mono_mass>
</ptm>
<ptm>
  <name>glycerylphosphorylethanolamine</name>
  <abbreviation>GlycerylPE</abbreviation>
  <unimod>396</unimod>
  <mono_mass>197.045310</mono_mass>
</ptm>
<ptm>
  <name>dipyrrolylmethanemethyl</name>
  <abbreviation>Dipyrrolylmethanemethyl</abbreviation>
  <unimod>416</unimod>
  <mono_mass>418.137616</mono_mass>
</ptm>
<ptm>
  <name>N-pyruvic acid 2-iminyl</name>
  <abbreviation>PyruvicAcidIminyl</abbreviation>
  <unimod>422</unimod>
  <mono_mass>70.005479</mono_mass>
</ptm>
<ptm>
  <name>persulfide</name>
  <abbreviation>Sulfide</abbreviation>
  <unimod>421</unimod>
  <mono_mass>31.972071</mono_mass>
</ptm>
<ptm>
  <name>Formylation</name>
  <abbreviation>Formyl</abbreviation>
  <unimod>122</unimod>
  <mono_mass>27.994915</mono_mass>
</ptm>
<ptm>
  <name>Palmitoylation</name>
  <abbreviation>Palmitoyl</abbreviation>
  <unimod>47</unimod>
  <mono_mass>238.229666</mono_mass>
</ptm>
<ptm>
  <name>diacylglycerol</name>
  <abbreviation>Diacylglycerol</abbreviation>
  <unimod>377</unimod>
  <mono_mass>576.511761</mono_mass>
</ptm>
<ptm>
  <name>Myristoylation</name>
  <abbreviation>Myristoyl</abbreviation>
  <unimod>45</unimod>
  <mono_mass>210.198366</mono_mass>
</ptm>
<ptm>
  <name>Geranyl-geranyl</name>
  <abbreviation>GeranylGeranyl</abbreviation>
  <unimod>48</unimod>
  <mono_mass>272.250401</mono_mass>
</ptm>
<ptm>
  <name>Farnesylation</name>
  <abbreviation>Farnesyl</abbreviation>
  <unimod>44</unimod>
  <mono_mass>204.187801</mono_mass>
</ptm>
<ptm>
  <name>glycosylphosphatidylinositol</name>
  <abbreviation>GPIanchor</abbreviation>
  <unimod>394</unimod>
  <mono_mass>123.008530</mono_mass>
</ptm>
</ptm_list> 
)";

} 

#endif
