

#if __ARM_FEATURE_SVE2
#include <arm_sve.h>


#if __ARM_FEATURE_SVE_BITS == 128 && __ARM_FEATURE_SVE_VECTOR_OPERATORS

typedef svuint64_t vint64x2_t __attribute__((arm_sve_vector_bits(__ARM_FEATURE_SVE_BITS)));

#endif

svuint64_t uaddlb_array(svuint32_t Zs1, svuint32_t Zs2)
{
    svuint64_t result = svaddlb(Zs1, Zs2);
    return result;
}

vint64x2_t uaddlb_array2(vint64x2_t Zs1, vint64x2_t Zs2)
{
    vint64x2_t result = Zs1 + Zs2;
    return result;
}

#endif