#include <iostream>
#include <cstdint>
#include "stdio.h"
#include <cstring>
#include <mmintrin.h>

__m64 setPacked8bitRegister(__int8_t *arr) 
{
    uint64_t bytes; 
    memcpy(&bytes, arr, 8);
    __m64 mm = _m_from_int64(bytes);
    return mm;
}

void calculate(__int8_t *A,__int8_t *B,__int8_t *C,__int16_t *D)
{
    uint64_t bytes, pt1, pt2;
    __m64 cmp_result, zero_vector = _mm_setzero_si64(), mm0, mm1;
 
    mm0 = setPacked8bitRegister(B);
    cmp_result = _m_pcmpgtb(zero_vector, mm0); //compare packed 8-bit int, 0 if mm0>zero_vector
    __m64 b_low = _m_punpcklbw(mm0, cmp_result); 
    __m64 b_high = _m_punpckhbw(mm0, cmp_result);

    mm1 = setPacked8bitRegister(C);
    cmp_result = _m_pcmpgtb(zero_vector, mm1);
    __m64 c_low = _m_punpcklbw(mm1, cmp_result); 
    __m64 c_high = _m_punpckhbw(mm1, cmp_result);
    
    __m64 bc_1 = _m_pmullw(b_low, c_low);
    __m64 bc_2 = _m_pmullw(b_high, c_high);

    mm0 = setPacked8bitRegister(A);
    cmp_result = _m_pcmpgtb(zero_vector, mm0);
    __m64 a_low = _m_punpcklbw(mm0, cmp_result);
    __m64 a_high = _m_punpckhbw(mm0, cmp_result);

    memcpy(&pt1, D, sizeof(bytes));
    __m64 d_low = _m_from_int64(pt1);
    memcpy(&pt2, &D[4], sizeof(bytes));
    __m64 d_high = _m_from_int64(pt2);

    a_low = _m_psubsw(a_low, bc_1);
    a_high = _m_psubsw(a_high, bc_2);

    a_low = _m_psubsw(a_low, d_low);
    a_high = _m_psubsw(a_high, d_high);

    int16_t F[8];
    memcpy(F, &a_low, sizeof(a_low));
    memcpy(&F[4], &a_high, sizeof(a_high));

    std::cout << "MMX: ";
    for(int i = 0; i < 8; i++)
    {
        std::cout << F[i] << " ";  
    }  
    
    std::cout << std::endl;
    
    F[8] = { };
    
    std::cout << "C++: ";
    for(int i = 0; i < 8; i++)
    {
        F[i] = (int)A[i]-(int)B[i]*(int)C[i]-(int)D[i]; 
        std::cout << F[i] << " ";  
    }
    
    std::cout << std::endl;
}

int main()
{
    int8_t A[8] = { 3, 7, -5, 6, 7, 8, 9, 100 };
    int8_t B[8] = { 1, 1, -1, 50, 1, 90, 1, 2 };
    int8_t C[8] = { 1, 3, 4, 1, -1, 6, 1, 15 };
    int16_t D[8] = { 1, 1, 745, 30000, -30000, 1, 1, 10 };

    calculate(A, B, C, D);
    
    return 0;
}
