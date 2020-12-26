#include <iostream>
#include <cstring>
#include <mmintrin.h>

__m64 setPacked8BitRegister(__int8_t *eightBitArray) 
{
    uint64_t bytes; 
    memcpy(&bytes, eightBitArray, sizeof(bytes));
    __m64 mm = _m_from_int64(bytes);
    return mm;
}

void Calculate(__int8_t *A, __int8_t *B, __int8_t *C, __int16_t *D)
{
    uint64_t bytes, firstPartOfD, secondPartOfD;
    __m64 cmpResult, zeroVector = _mm_setzero_si64(), mm0, mm1;
 
    mm0 = setPacked8BitRegister(A);
    cmpResult = _m_pcmpgtb(zeroVector, mm0);
    __m64 aLow = _mm_unpacklo_pi8(mm0, cmpResult); 
    __m64 aHigh = _mm_unpackhi_pi8(mm0, cmpResult);

    mm1 = setPacked8BitRegister(C);
    cmpResult = _m_pcmpgtb(zeroVector, mm1);
    __m64 cLow = _m_punpcklbw(mm1, cmpResult); 
    __m64 cHigh = _m_punpckhbw(mm1, cmpResult);
    
    __m64 firstPartOfAC = _m_pmullw(aLow, cLow);
    __m64 secondPartOfAC = _m_pmullw(aHigh, cHigh);

    mm0 = setPacked8BitRegister(B);
    cmpResult = _m_pcmpgtb(zeroVector, mm0);
    __m64 bLow = _m_punpcklbw(mm0, cmpResult);
    __m64 bHigh = _m_punpckhbw(mm0, cmpResult);

    memcpy(&firstPartOfD, D, sizeof(bytes));
    memcpy(&secondPartOfD, &D[4], sizeof(bytes));
    __m64 dLow = _m_from_int64(firstPartOfD);
    __m64 dHigh = _m_from_int64(secondPartOfD);

    __m64 firstPartOfBD = _mm_mullo_pi16(bLow, dLow);
    __m64 secondPartOfBD = _mm_mullo_pi16(bHigh, dHigh);
	
    aLow = _mm_add_pi16(firstPartOfAC, firstPartOfBD);
    aHigh = _mm_add_pi16(secondPartOfAC, secondPartOfBD);

    int16_t F[8];
    memcpy(F, &aLow, sizeof(aLow));
    memcpy(&F[4], &aHigh, sizeof(aHigh));

    std::cout << "MMX Result: ";
    for(int i = 0; i < sizeof(F)/sizeof(*F); i++)
    {
        std::cout << F[i] << " ";  
    }  
    
    std::cout << std::endl;
    
    F[8] = { };
    
    std::cout << "C++ Result: ";
    for(int i = 0; i < sizeof(F)/sizeof(*F); i++)
    {
        F[i] = (int)A[i]*(int)C[i]+(int)B[i]*(int)D[i]; 
        std::cout << F[i] << " ";  
    }
    
    std::cout << std::endl;
}

int main()
{
    int8_t A[8] = { 3, 7, -5, 69, 7, 8, 9, 99 };
    int8_t B[8] = { 64, 1, -1, 50, 1, 90, 69, 2 };
    int8_t C[8] = { 1, 3, 4, 1, -1, 6, 1, 15 };
    int16_t D[8] = { 1, -264, 745, 30000, -30000, 587, 1, 269 };

    Calculate(A, B, C, D);
    
    return 0;
}
