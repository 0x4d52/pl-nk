#ifndef CUSTOMVECTORS_H_INCLUDED
#define CUSTOMVECTORS_H_INCLUDED

#include "ipp.h"
#include "mkl.h"
#include <xmmintrin.h>

#define PLANK_VEC_CUSTOM    1

#define PLANK_SIMDF_LENGTH  4   // vector 4 floats
#define PLANK_SIMDF_SIZE   16
#define PLANK_SIMDF_SHIFT   2   // divide by 4 for length
#define PLANK_SIMDF_MASK    3   // remainder mask for non-multiples of 4
typedef __m128 PlankVF;

#define PLANK_SIMDD_LENGTH  2   // vector 2 doubles
#define PLANK_SIMDD_SIZE   16
#define PLANK_SIMDD_SHIFT   1   // divide by 2 for length
#define PLANK_SIMDD_MASK    1   // remainder mask for non-even lengths
typedef __m128d PlankVD;

#define PLANK_SIMDI_LENGTH  4   // vector 4 ints
#define PLANK_SIMDI_SIZE   16
#define PLANK_SIMDI_SHIFT   2   // divide by 4 for length
#define PLANK_SIMDI_MASK    3   // remainder mask for non-multiples of 4
//typedef __v4si PlankVI;
typedef __m128 PlankVI;

#define PLANK_SIMDS_LENGTH  8   // vector 8 shorts
#define PLANK_SIMDS_SIZE   16
#define PLANK_SIMDS_SHIFT   3   // divide by 8 for length
#define PLANK_SIMDS_MASK    7   // remainder mask for non-multiples of 8
//typedef __v8hi PlankVS;
typedef __m128 PlankVS;

#if PLANK_ARM || PLANK_PPC // no 64-bit int SIMD
#define PLANK_SIMDLL_LENGTH  1   // vector 1 LongLong
#define PLANK_SIMDLL_SIZE    8
#define PLANK_SIMDLL_SHIFT   0   // no shift
#define PLANK_SIMDLL_MASK    0   // no remainder
typedef PlankLL PlankVLL;
#else
#define PLANK_SIMDLL_LENGTH  2   // vector 2 LongLongs
#define PLANK_SIMDLL_SIZE   16
#define PLANK_SIMDLL_SHIFT   1   // divide by 2 for length
#define PLANK_SIMDLL_MASK    1   // remainder mask for non-even lengths
typedef __m128i PlankVLL;
#endif

#define PLANK_SIMDF_Set(A)   _mm_set_ps(A,A,A,A)
#define PLANK_SIMDF_Mul(A,B) _mm_mul_ps(A,B)
#define PLANK_SIMDF_Div(A,B) _mm_div_ps(A,B)
#define PLANK_SIMDF_Add(A,B) _mm_add_ps(A,B)
#define PLANK_SIMDF_Sub(A,B) _mm_sub_ps(A,B)

#define PLANK_SIMDI_Set(A)   _mm_set_ps(A,A,A,A)
#define PLANK_SIMDI_Add(A,B) _mm_add_epi32(A,B)
#define PLANK_SIMDI_Sub(A,B) _mm_sub_epi32(A,B)


//------------------------------- float ----------------------------------------

static PLANK_INLINE_MID float pl_VectorMeanF_N (const float *a, PlankUL N)
{
    Ipp32f mean;
    ippsMean_32f (a, (int)N, &mean, ippAlgHintFast);
    return (float)mean;
}

static PLANK_INLINE_MID void pl_VectorFillF_N1 (float *result, float a, PlankUL N)
{
    ippsSet_32f (a, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorClearF_N (float *result, PlankUL N)
{
    ippsZero_32f (result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorMoveF_NN (float *result, const float* a, PlankUL N)
{
    ippsMove_32f (a, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorIncF_NN (float *result, const float* a, PlankUL N)
{
    ippsSet_32f (1.0f, result, (int)N);
    vsAdd ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorDecF_NN (float *result, const float* a, PlankUL N)
{
    ippsSet_32f (-1.0f, result, (int)N);
    vsAdd ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorNegF_NN (float *result, const float* a, PlankUL N)
{
    ippsSet_32f (-1.0f, result, (int)N);
    vsMul ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorAbsF_NN (float *result, const float* a, PlankUL N)
{
    vsAbs ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorReciprocalF_NN (float *result, const float* a, PlankUL N)
{
    vsInv ((int)N, a, result);
}

PLANK_VECTORUNARYOP_DEFINE(Log2,F)

static PLANK_INLINE_MID void pl_VectorSinF_NN (float *result, const float* a, PlankUL N)
{
    vsSin ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorCosF_NN (float *result, const float* a, PlankUL N)
{
    vsCos ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorTanF_NN (float *result, const float* a, PlankUL N)
{
    vsTan ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorAsinF_NN (float *result, const float* a, PlankUL N)
{
    vsAsin ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorAcosF_NN (float *result, const float* a, PlankUL N)
{
    vsAcos ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorAtanF_NN (float *result, const float* a, PlankUL N)
{
    vsAtan ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorSinhF_NN (float *result, const float* a, PlankUL N)
{
    vsSinh ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorCoshF_NN (float *result, const float* a, PlankUL N)
{
    vsCosh ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorTanhF_NN (float *result, const float* a, PlankUL N)
{
    vsTanh ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorSqrtF_NN (float *result, const float* a, PlankUL N)
{
    vsSqrt ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorLogF_NN (float *result, const float* a, PlankUL N)
{
    vsLn ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorLog10F_NN (float *result, const float* a, PlankUL N)
{
    vsLog10 ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorExpF_NN (float *result, const float* a, PlankUL N)
{
    vsExp ((int)N, a, result);
}

static PLANK_INLINE_MID void pl_VectorCeilF_NN (float *result, const float* a, PlankUL N)
{
    ippsCeil_32f (a, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorFloorF_NN (float *result, const float* a, PlankUL N)
{
    ippsFloor_32f (a, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorPowF_NNN (float *result, const float* a, const float *b, PlankUL N)
{
    vsPow ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorPowF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    ippsSet_32f (b, result, (int)N);
    vsPow ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorPowF_N1N (float *result, float a, const float* b, PlankUL N)
{
    ippsSet_32f (a, result, (int)N);
    vsPow ((int)N, result, b, result);
}

static PLANK_INLINE_MID void pl_VectorAtan2F_NNN (float *result, const float* a, const float *b, PlankUL N)
{
    vsAtan2 ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorAtan2F_NN1 (float *result, const float* a, float b, PlankUL N)
{
    ippsSet_32f (b, result, (int)N);
    vsAtan2 ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorAtan2F_N1N (float *result, float a, const float* b, PlankUL N)
{
    ippsSet_32f (a, result, (int)N);
    vsAtan2 ((int)N, result, b, result);
}

PLANK_VECTORBINARYOP_DEFINE(IsEqualTo,F)
PLANK_VECTORBINARYOP_DEFINE(IsNotEqualTo,F)
PLANK_VECTORBINARYOP_DEFINE(IsGreaterThan,F)
PLANK_VECTORBINARYOP_DEFINE(IsGreaterThanOrEqualTo,F)
PLANK_VECTORBINARYOP_DEFINE(IsLessThan,F)
PLANK_VECTORBINARYOP_DEFINE(IsLessThanOrEqualTo,F)


PLANK_VECTORBINARYOP_DEFINE(SumSqr,F)
PLANK_VECTORBINARYOP_DEFINE(DifSqr,F)
PLANK_VECTORBINARYOP_DEFINE(SqrSum,F)
PLANK_VECTORBINARYOP_DEFINE(SqrDif,F)
PLANK_VECTORBINARYOP_DEFINE(AbsDif,F)
PLANK_VECTORBINARYOP_DEFINE(Thresh,F)

static PLANK_INLINE_MID void pl_VectorSquaredF_NN (float *result, const float* a, PlankUL N)
{
    ippsSqr_32f_A24 (a, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorCubedF_NN (float *result, const float* a, PlankUL N)
{
    ippsSqr_32f_A24 (a, result, (int)N);
    vsMul ((int)N, a, result, result);
}

static PLANK_INLINE_MID void pl_VectorFracF_NN (float *result, const float* a, PlankUL N)
{
    vsFrac((int)N, a, result);
}

PLANK_VECTORUNARYOP_DEFINE(Sign, F)

PLANK_VECTORUNARYOP_DEFINE(M2F, F)
PLANK_VECTORUNARYOP_DEFINE(F2M, F)
PLANK_VECTORUNARYOP_DEFINE(A2dB, F)
PLANK_VECTORUNARYOP_DEFINE(dB2A, F)
PLANK_VECTORUNARYOP_DEFINE(D2R, F)
PLANK_VECTORUNARYOP_DEFINE(R2D, F)
PLANK_VECTORUNARYOP_DEFINE(Distort, F)
PLANK_VECTORUNARYOP_DEFINE(Zap, F)

#define PLANK_CUSTOM_SIMD_LOOP(OP)\
    PlankVF* vR;\
    PlankVF* vA;\
    PlankVF vB;\
    PlankUL vN, Nr;\
    \
    Nr  = N & PLANK_SIMDF_MASK;\
    \
    if (! Nr)\
    {\
        vN  = N >> PLANK_SIMDF_SHIFT;\
        vR  = (PlankVF*)result;\
        vA  = (PlankVF*)a;\
        vB  = PLANK_SIMDF_Set(b);\
        \
        while (vN--)\
            *vR++ = PLANK_SIMDF_##OP(*vA++,vB);\
    }\
    else\
    {\
        while (N--)\
            *result++ = pl_##OP##F (*a++, b);\
    }



static PLANK_INLINE_MID void pl_VectorAddF_NNN (float *result, const float* a, const float* b, PlankUL N)
{
    vsAdd ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorAddF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    PLANK_CUSTOM_SIMD_LOOP(Add)
}

static PLANK_INLINE_MID void pl_VectorAddF_N1N (float *result, float a, const float* b, PlankUL N)
{
    pl_VectorAddF_NN1 (result, b, a, N);
}

static PLANK_INLINE_MID void pl_VectorSubF_NNN (float *result, const float* a, const float* b, PlankUL N)
{
    vsSub ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorSubF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    PLANK_CUSTOM_SIMD_LOOP(Sub)
}

static PLANK_INLINE_MID void pl_VectorSubF_N1N (float *result, float a, const float* b, PlankUL N)
{
    pl_VectorSubF_NN1 (result, b, a, N);
}

static PLANK_INLINE_MID void pl_VectorMulF_NNN (float *result, const float* a, const float* b, PlankUL N)
{
    vsMul ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorMulF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    PLANK_CUSTOM_SIMD_LOOP(Mul)
}

static PLANK_INLINE_MID void pl_VectorMulF_N1N (float *result, float a, const float* b, PlankUL N)
{
    pl_VectorMulF_NN1 (result, b, a, N);
}

static PLANK_INLINE_MID void pl_VectorDivF_NNN (float *result, const float* a, const float* b, PlankUL N)
{
    vsDiv ((int)N, a, b, result);
}

static PLANK_INLINE_MID void pl_VectorDivF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    PLANK_CUSTOM_SIMD_LOOP(Div)
}

static PLANK_INLINE_MID void pl_VectorDivF_N1N (float *result, float a, const float* b, PlankUL N)
{
    pl_VectorDivF_NN1 (result, b, a, N);
}

#undef PLANK_CUSTOM_SIMD_LOOP

PLANK_VECTORBINARYOP_DEFINE(Mod,F)


static PLANK_INLINE_MID void pl_VectorRampF_N11 (float *result, float a, float b, PlankUL N)
{
    ippsVectorSlope_32f (result, (int)N, a, b);
}

//{
//    PlankVF vb;
//    PlankVF *vx;
//    PlankVF *vy;
//    PlankUL i, Nvec, Nrem;
//    float* vbp;
//    float bv;
//    
//    Nrem = N & PLANK_SIMDF_MASK;
//
//    if (Nrem == 0)
//    {
//        vx  = (PlankVF*)result;
//        vbp = (float*)&vb;
//        bv  = b * PLANK_SIMDF_LENGTH;
//        
//        for (i = 0; i < PLANK_SIMDF_LENGTH; PLANK_INC(i))
//        {
//            vbp[i]    = bv;
//            *result++ = a;
//            a += b;
//        }
//        
//        vy   = (PlankVF*)result;
//        Nvec = (N / PLANK_SIMDF_LENGTH) - 1;
//        
//        for (i = 0; i < Nvec; PLANK_INC(i))
//            *vy++ = *vx++ + vb;
//    }
//    else
//    {
//        for (i = 0; i < N; PLANK_INC(i))
//        {
//            result[i] = a;
//            a += b;
//        }
//    }
//}

static PLANK_INLINE_MID void pl_VectorRampMulF_N11 (float *result, float a, float b, PlankUL N)
{
    PlankVF vb;
    PlankVF va;
    PlankVF *vr;
    PlankUL i, Nvec, Nrem;
    float* vbp;
    float* vap;
    float bv;
    
    Nrem = N & PLANK_SIMDF_MASK;
    
    if (Nrem == 0)
    {
        vr  = (PlankVF*)result;
        vbp = (float*)&vb;
        vap = (float*)&va;
        bv  = b * PLANK_SIMDF_LENGTH;
        
        vb = PLANK_SIMDF_Set(b);
        
        for (i = 0; i < PLANK_SIMDF_LENGTH; PLANK_INC(i))
        {
            vap[i] = a;
            a += b;
        }
        
        Nvec = N / PLANK_SIMDF_LENGTH;
        
        for (i = 0; i < Nvec; PLANK_INC(i))
        {
            *vr = PLANK_SIMDF_Mul (*vr, va);   // vr[i] *= va;
            va = PLANK_SIMDF_Add (va, vb);     // va += vb;
            ++vr;
        }
    }
    else
    {
        for (i = 0; i < N; PLANK_INC(i))
        {
            result[i] *= a;
            a += b;
        }
    }
}

static PLANK_INLINE_MID void pl_VectorLineF_N11 (float *result, float a, float b, PlankUL N)
{
    pl_VectorRampF_N11 (result, a, (b - a) / (N - 1), N);
}

PLANK_VECTORBINARYOPVECTOR_DEFINE(Min,F)


static PLANK_INLINE_MID void pl_VectorMinF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    ippsThreshold_32f (a, result, (int)N, b, ippCmpGreater);
}

static PLANK_INLINE_MID void pl_VectorMinF_N1N (float *result, float a, const float* b, PlankUL N)
{
    ippsThreshold_32f (b, result, (int)N, a, ippCmpGreater);
}

PLANK_VECTORBINARYOPVECTOR_DEFINE(Max,F)

static PLANK_INLINE_MID void pl_VectorMaxF_NN1 (float *result, const float* a, float b, PlankUL N)
{
    ippsThreshold_32f (a, result, (int)N, b, ippCmpLess);
}

static PLANK_INLINE_MID void pl_VectorMaxF_N1N (float *result, float a, const float* b, PlankUL N)
{
    ippsThreshold_32f (b, result, (int)N, a, ippCmpLess);
}

PLANK_VECTORBINARYOP_DEFINE(Hypot,F)

static PLANK_INLINE_MID void pl_VectorMulAddF_NNNN (float *result, const float* input, const float* a, const float* b, PlankUL N)
{
    ippsCopy_32f (input, result, (int)N);
    ippsAddProduct_32f (a, b, result, (int)N);
}

static PLANK_INLINE_MID void pl_VectorMulAddF_NNN (float *io, const float* a, const float* b, PlankUL N)
{
    ippsAddProduct_32f (a, b, io, (int)N);
}

PLANK_VECTORMULSCALARADD_DEFINE(F);
PLANK_VECTORSCALARMULSCALARADD_DEFINE(F);

static PLANK_INLINE_MID void pl_VectorMulAddF_NN1N (float *result, const float* input, float a, const float* b, PlankUL N)
{
    ippsCopy_32f (input, result, (int)N);
    
}

static PLANK_INLINE_MID void pl_VectorAddMulF_1NN (float *result, const float* a, const float* b, PlankUL N)
{
    ippsDotProd_32f (a, b, (int)N, result);
}

//static PLANK_INLINE_MID void pl_VectorZMulF_ZNNNNNN (float *resultReal, float *resultImag,
//                                                     const float* leftReal, const float* leftImag,
//                                                     const float* rightReal, const float* rightImag,
//                                                     PlankUL N)
//{
//    PlankUL i;
//    PlankVF *Vor, *Voi, *Vlr, *Vli, *Vrr, *Vri;
//    
//    Vor = (PlankVF*)resultReal;
//    Voi = (PlankVF*)resultImag;
//    Vlr = (PlankVF*)leftReal;
//    Vli = (PlankVF*)leftImag;
//    Vrr = (PlankVF*)rightReal;
//    Vri = (PlankVF*)rightImag;
//    
//    N = N / 4;
//    
//    for (i = 0; i < N; PLANK_INC(i))
//    {
//        Vor[i] = PLANK_SIMDF_Sub (PLANK_SIMDF_Mul (Vlr[i], Vrr[i]), PLANK_SIMDF_Mul (Vli[i], Vri[i]));
//        Voi[i] = PLANK_SIMDF_Add (PLANK_SIMDF_Mul (Vlr[i], Vri[i]), PLANK_SIMDF_Mul (Vli[i], Vrr[i]));
//    }
//}

static PLANK_INLINE_MID void pl_VectorZMulF_ZNNNNNN (float *resultReal, float *resultImag,
                                                     const float* leftReal, const float* leftImag,
                                                     const float* rightReal, const float* rightImag,
                                                     PlankUL N)
{
    PlankVF *Vor, *Voi, *Vlr, *Vli, *Vrr, *Vri;
    
    Vor = (PlankVF*)resultReal;
    Voi = (PlankVF*)resultImag;
    Vlr = (PlankVF*)leftReal;
    Vli = (PlankVF*)leftImag;
    Vrr = (PlankVF*)rightReal;
    Vri = (PlankVF*)rightImag;
    
    N = N / 4;
    
    while (N--)
    {
        *Vor++ = PLANK_SIMDF_Sub (PLANK_SIMDF_Mul (*Vlr, *Vrr), PLANK_SIMDF_Mul (*Vli, *Vri));
        *Voi++ = PLANK_SIMDF_Add (PLANK_SIMDF_Mul (*Vlr, *Vri), PLANK_SIMDF_Mul (*Vli, *Vrr));
        ++Vlr;
        ++Vrr;
        ++Vli;
        ++Vri;
    }
}

static PLANK_INLINE_MID void pl_VectorZMulAddF_ZNNNNNNNN (float *resultReal, float *resultImag,
                                                          const float* inputReal, const float* inputImag,
                                                          const float* mulReal, const float* mulImag,
                                                          const float* addReal, const float* addImag,
                                                          PlankUL N)
{
    PlankVF *Vor, *Voi, *Vir, *Vii, *Vmr, *Vmi, *Var, *Vai;

    Vor = (PlankVF*)resultReal;
    Voi = (PlankVF*)resultImag;
    Vir = (PlankVF*)inputReal;
    Vii = (PlankVF*)inputImag;
    Vmr = (PlankVF*)mulReal;
    Vmi = (PlankVF*)mulImag;
    Var = (PlankVF*)addReal;
    Vai = (PlankVF*)addImag;
    
    N = N / 4;
    
    while (N--)
    {
        *Vor++ = PLANK_SIMDF_Add (PLANK_SIMDF_Sub (PLANK_SIMDF_Mul (*Vir, *Vmr), PLANK_SIMDF_Mul (*Vii, *Vmi)), *Var);
        *Voi++ = PLANK_SIMDF_Add (PLANK_SIMDF_Add (PLANK_SIMDF_Mul (*Vir, *Vmi), PLANK_SIMDF_Mul (*Vii, *Vmr)), *Vai);
        ++Vir;
        ++Vmr;
        ++Vii;
        ++Vmi;
        ++Var;
        ++Vai;
    }
}


PLANK_VECTORLOOKUP_DEFINE(F)


static PLANK_INLINE_MID void pl_VectorInterleave2F_Nnn (float *result, const float* splitA, const float* splitB, PlankUL n)
{
    IppiSize roiSize;
    roiSize.width  = n * 2;
    roiSize.height = 1;
    
    ippiCopy_32f_C1R (splitA, 1, result, 2, roiSize);
    ippiCopy_32f_C1R (splitB, 1, result + 1, 2, roiSize);
}

static PLANK_INLINE_MID void pl_VectorDeinterleave2F_nnN (float *resultA, float* resultB, const float* input, PlankUL n)
{
    IppiSize roiSize;
    roiSize.width  = n * 2;
    roiSize.height = 1;
    
    ippiCopy_32f_C1R (input, 2, resultA, 1, roiSize);
    ippiCopy_32f_C1R (input + 1, 2, resultB, 1, roiSize);
}


//------------------------------- double ---------------------------------------


PLANK_VECTOR_OPS_ALL(D)
PLANK_VECTORZMUL_DEFINE(D)
PLANK_VECTORZMULADD_DEFINE(D)
PLANK_VECTORINTERLEAVE2_DEFINE(D)
PLANK_VECTORDEINTERLEAVE2_DEFINE(D)


#undef I



static PLANK_INLINE_MID void pl_VectorConvertD2F_NN (float *result, const double* input, PlankUL N)
{
    ippsConvert_64f32f (input, result, (int)N);
}

PLANK_VECTORCONVERT_DEFINE(F,C);

static PLANK_INLINE_MID void pl_VectorConvertI2F_NN (float *result, const int* input, PlankUL N)
{
    ippsConvert_32s32f_Sfs (input, result, (int)N, 1);
}

static PLANK_INLINE_MID void pl_VectorConvertS2F_NN (float *result, const short* input, PlankUL N)
{
    ippsConvert_16s32f_Sfs (input, result, (int)N, 1);
}

static PLANK_INLINE_MID void pl_VectorConvertF2D_NN (double *result, const float* input, PlankUL N)
{
    ippsConvert_32f64f (input, result, (int)N);
}

PLANK_VECTORCONVERT_DEFINE(D,C);


static PLANK_INLINE_MID void pl_VectorConvertI2D_NN (double *result, const int* input, PlankUL N)
{
    ippsConvert_32s64f_Sfs(input, result, (int)N, 1);
}

static PLANK_INLINE_MID void pl_VectorConvertS2D_NN (double *result, const short* input, PlankUL N)
{
    ippsConvert_16s64f_Sfs (input, result, (int)N, 1);
}

PLANK_VECTORCONVERT_DEFINE(C,F);
PLANK_VECTORCONVERT_DEFINE(C,D);
PLANK_VECTORCONVERT_DEFINE(C,I)
PLANK_VECTORCONVERT_DEFINE(C,S)

static PLANK_INLINE_MID void pl_VectorConvertF2I_NN (int *result, const float* input, PlankUL N)
{
    ippsConvert_32f32s_Sfs (input, result, (int)N, ippRndZero, 1);
}

static PLANK_INLINE_MID void pl_VectorConvertD2I_NN (int *result, const double* input, PlankUL N)
{
    ippsConvert_64f32s_Sfs (input, result, (int)N, ippRndZero, 1);
}

PLANK_VECTORCONVERT_DEFINE(I,C)
PLANK_VECTORCONVERT_DEFINE(I,S)

static PLANK_INLINE_MID void pl_VectorConvertF2S_NN (short *result, const float* input, PlankUL N)
{
    ippsConvert_32f16s_Sfs (input, result, (int)N, ippRndZero, 1);
}

static PLANK_INLINE_MID void pl_VectorConvertD2S_NN (short *result, const double* input, PlankUL N)
{
    ippsConvert_64f16s_Sfs (input, result, (int)N, ippRndZero, 1);
}

PLANK_VECTORCONVERT_DEFINE(S,C)
PLANK_VECTORCONVERT_DEFINE(S,I)

PLANK_VECTORCONVERT_DEFINE(LL,C)
PLANK_VECTORCONVERT_DEFINE(LL,I)
PLANK_VECTORCONVERT_DEFINE(LL,S)
PLANK_VECTORCONVERT_DEFINE(LL,F)
PLANK_VECTORCONVERT_DEFINE(LL,D)
PLANK_VECTORCONVERT_DEFINE(C,LL)
PLANK_VECTORCONVERT_DEFINE(I,LL)
PLANK_VECTORCONVERT_DEFINE(S,LL)
PLANK_VECTORCONVERT_DEFINE(F,LL)
PLANK_VECTORCONVERT_DEFINE(D,LL)


PLANK_VECTORCONVERTROUNDF_DEFINE(C)
PLANK_VECTORCONVERTROUNDF_DEFINE(I)
PLANK_VECTORCONVERTROUNDF_DEFINE(S)
PLANK_VECTORCONVERTROUNDF_DEFINE(LL)
PLANK_VECTORCONVERTROUNDD_DEFINE(C)
PLANK_VECTORCONVERTROUNDD_DEFINE(I)
PLANK_VECTORCONVERTROUNDD_DEFINE(S)
PLANK_VECTORCONVERTROUNDD_DEFINE(LL)


// short
PLANK_VECTOR_OPS_COMMON(S)

// int
PLANK_VECTORFILL_DEFINE(I)
PLANK_VECTORCLEAR_DEFINE(I)
PLANK_VECTORRAMP_DEFINE(I)
PLANK_VECTORRAMPMUL_DEFINE(I)
PLANK_VECTORLINE_DEFINE(I)
PLANK_VECTORMEAN_DEFINE(I)

PLANK_VECTORUNARYOP_DEFINE(Move,I)
PLANK_VECTORUNARYOP_DEFINE(Inc,I)
PLANK_VECTORUNARYOP_DEFINE(Dec,I)
PLANK_VECTORUNARYOP_DEFINE(Neg,I)
PLANK_VECTORUNARYOP_DEFINE(Abs,I)
PLANK_VECTORUNARYOP_DEFINE(Squared,I)
PLANK_VECTORUNARYOP_DEFINE(Cubed,I)
PLANK_VECTORUNARYOP_DEFINE(Sign,I)

static PLANK_INLINE_MID void pl_VectorAddI_NNN (int *result, const int* a, const int* b, PlankUL N)
{
    PlankUL vN, Nr, i;
    PlankVI* vResult;
    PlankVI* vA;
    PlankVI* vB;
    
    vN = N >> PLANK_SIMDI_SHIFT;
    Nr = N & PLANK_SIMDI_MASK;
    
    vResult = (PlankVI*)result;
    vA = (PlankVI*)a;
    vB = (PlankVI*)b;
    
    for (i = 0; i < vN; ++i)
        *vResult++ = PLANK_SIMDI_Add(*vA++, *vB++);
    
    if (Nr)
    {
        result = (int*)vResult;
        a      = (int*)vA;
        b      = (int*)vB;
        
        for (i = 0; i < Nr; ++i)
            *result++ = *a++ + *b++;
    }
}

static PLANK_INLINE_MID void pl_VectorAddI_NN1 (int *result, const int* a, int b, PlankUL N)
{
    PlankVI vB;
    PlankUL vN, Nr, i;
    PlankVI* vResult;
    PlankVI* vA;
    
    vN      = N >> PLANK_SIMDI_SHIFT;
    Nr      = N & PLANK_SIMDI_MASK;
    vResult = (PlankVI*)result;
    vA      = (PlankVI*)a;
    vB      = PLANK_SIMDI_Set(b);
    
    for (i = 0; i < vN; ++i)
        *vResult++ = PLANK_SIMDI_Add(*vA++, vB);
    
    if (Nr)
    {
        result = (int*)vResult;
        a       = (int*)vA;
        
        for (i = 0; i < Nr; ++i)
            *result++ = *a++ + b;
    }
}

static PLANK_INLINE_MID void pl_VectorAddI_N1N (int *result, int a, const int* b, PlankUL N)
{
    PlankVI vA;
    PlankUL vN, Nr, i;
    PlankVI* vResult;
    PlankVI* vB;
    
    vN      = N >> PLANK_SIMDI_SHIFT;
    Nr      = N & PLANK_SIMDI_MASK;
    vResult = (PlankVI*)result;
    vB      = (PlankVI*)b;
    vA      = PLANK_SIMDI_Set(a);
    
    for (i = 0; i < vN; ++i)
        *vResult++ = PLANK_SIMDI_Add(vA, *vB++);
    
    if (Nr)
    {
        result = (int*)vResult;
        b      = (int*)vB;
        
        for (i = 0; i < Nr; ++i)
            *result++ = a + *b++;
    }
}


PLANK_VECTORBINARYOP_DEFINE(Sub,I)
PLANK_VECTORBINARYOP_DEFINE(Mul,I)
PLANK_VECTORBINARYOP_DEFINE(Div,I)
PLANK_VECTORBINARYOP_DEFINE(Mod,I)
PLANK_VECTORBINARYOP_DEFINE(Min,I)
PLANK_VECTORBINARYOP_DEFINE(Max,I)

PLANK_VECTORBINARYOP_DEFINE(IsEqualTo,I)
PLANK_VECTORBINARYOP_DEFINE(IsNotEqualTo,I)
PLANK_VECTORBINARYOP_DEFINE(IsGreaterThan,I)
PLANK_VECTORBINARYOP_DEFINE(IsGreaterThanOrEqualTo,I)
PLANK_VECTORBINARYOP_DEFINE(IsLessThan,I)
PLANK_VECTORBINARYOP_DEFINE(IsLessThanOrEqualTo,I)

PLANK_VECTORBINARYOP_DEFINE(SumSqr,I)
PLANK_VECTORBINARYOP_DEFINE(DifSqr,I)
PLANK_VECTORBINARYOP_DEFINE(SqrSum,I)
PLANK_VECTORBINARYOP_DEFINE(SqrDif,I)
PLANK_VECTORBINARYOP_DEFINE(AbsDif,I)
PLANK_VECTORBINARYOP_DEFINE(Thresh,I)

PLANK_VECTORMULADD_DEFINE(I)
PLANK_VECTORMULADDINPLACE_DEFINE(I)
PLANK_VECTORMULSCALARADD_DEFINE(I)
PLANK_VECTORSCALARMULSCALARADD_DEFINE(I)
PLANK_VECTORSCALARMULADD_DEFINE(I)

PLANK_VECTORLOOKUP_DEFINE(I)

// longlong
PLANK_VECTOR_OPS_COMMON(LL)




#endif  // CUSTOMVECTORS_H_INCLUDED
