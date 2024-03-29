### Result of the execution:

<p align="center">
  <img src="https://i.ibb.co/cycj71V/mmx-Result.png" title="hover text">
</p>

***For correct execution you should run it on Linux***

Initial solution contained **mm_setr_pi16** instructions. Then it was redone to set packed 8-bit (and 16-bit) registers using **memcpy**. </br>
To eliminate confusion it is worth mentioning that in some parts of the code different instructions do the same things. It is just an educational part. For instance, **mm_unpacklo_pi8** and **m_punpcklbw** differ only in their naming. You can check it yourself (Intel Intrinsics Guide): [mm_unpacklo_pi8](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=MMX&expand=4341,4015,4341,3514,131,130,4147,948,4148,4149,4446,6108&text=_mm_unpacklo_pi8), [m_punpcklbw](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=MMX&expand=4341,4015,4341,3514,131,130,948,4149,4446,4149,4148,4147,4444&text=m_punpcklbw). The same applies to [m_pmullw](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=MMX&expand=4341,4015,4341,3514,131,130,948,4149,4446,4149,4148,4147,4444,4341&text=_m_pmullw) and [mm_mullo_pi16](https://software.intel.com/sites/landingpage/IntrinsicsGuide/#techs=MMX&expand=4341,4015,4341,3514,131,130,4147,948,4148,4149,4446,6108,4015&text=_mm_mullo_pi16). </br>
Other than that, everything else should be clear. We pack a bunch of 8-bit (16-bit) numbers and store them in *m64* register by copying (*memcpy*) eight 8-bit numbers to uint64_t *bytes* variable. *m_pcmpgtb(m64 a, m64 b)* compares two packed 8-bit registers and yields 0 if b > a and gives the actual result in another case. Comparing it to the vector of zeroes helps us correctly handle negative numbers. Then we unpack registers and get low and high parts of them in order to perform respective mathematical operations.
