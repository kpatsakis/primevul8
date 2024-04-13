void VariationalRefinementImpl::ComputeDataTerm_ParBody::operator()(const Range &range) const
{
    int start_i = min(range.start * stripe_sz, h);
    int end_i = min(range.end * stripe_sz, h);

    float zeta_squared = var->zeta * var->zeta;
    float epsilon_squared = var->epsilon * var->epsilon;
    float gamma2 = var->gamma / 2;
    float delta2 = var->delta / 2;

    float *pIx, *pIy, *pIz;
    float *pIxx, *pIxy, *pIyy, *pIxz, *pIyz;
    float *pdU, *pdV;
    float *pa11, *pa12, *pa22, *pb1, *pb2;

    float derivNorm, derivNorm2;
    float Ik1z, Ik1zx, Ik1zy;
    float weight;
    int len;
    for (int i = start_i; i < end_i; i++)
    {
#define INIT_ROW_POINTERS(color)                                                                                       \
    pIx = var->Ix_rb.color.ptr<float>(i + 1) + 1;                                                                      \
    pIy = var->Iy_rb.color.ptr<float>(i + 1) + 1;                                                                      \
    pIz = var->Iz_rb.color.ptr<float>(i + 1) + 1;                                                                      \
    pIxx = var->Ixx_rb.color.ptr<float>(i + 1) + 1;                                                                    \
    pIxy = var->Ixy_rb.color.ptr<float>(i + 1) + 1;                                                                    \
    pIyy = var->Iyy_rb.color.ptr<float>(i + 1) + 1;                                                                    \
    pIxz = var->Ixz_rb.color.ptr<float>(i + 1) + 1;                                                                    \
    pIyz = var->Iyz_rb.color.ptr<float>(i + 1) + 1;                                                                    \
    pa11 = var->A11.color.ptr<float>(i + 1) + 1;                                                                       \
    pa12 = var->A12.color.ptr<float>(i + 1) + 1;                                                                       \
    pa22 = var->A22.color.ptr<float>(i + 1) + 1;                                                                       \
    pb1 = var->b1.color.ptr<float>(i + 1) + 1;                                                                         \
    pb2 = var->b2.color.ptr<float>(i + 1) + 1;                                                                         \
    pdU = dW_u->color.ptr<float>(i + 1) + 1;                                                                           \
    pdV = dW_v->color.ptr<float>(i + 1) + 1;                                                                           \
    if (i % 2 == 0)                                                                                                    \
        len = var->Ix_rb.color##_even_len;                                                                             \
    else                                                                                                               \
        len = var->Ix_rb.color##_odd_len;

        if (red_pass)
        {
            INIT_ROW_POINTERS(red);
        }
        else
        {
            INIT_ROW_POINTERS(black);
        }
#undef INIT_ROW_POINTERS

        int j = 0;
#if CV_SIMD128
        v_float32x4 zeta_vec = v_setall_f32(zeta_squared);
        v_float32x4 eps_vec = v_setall_f32(epsilon_squared);
        v_float32x4 delta_vec = v_setall_f32(delta2);
        v_float32x4 gamma_vec = v_setall_f32(gamma2);
        v_float32x4 zero_vec = v_setall_f32(0.0f);
        v_float32x4 pIx_vec, pIy_vec, pIz_vec, pdU_vec, pdV_vec;
        v_float32x4 pIxx_vec, pIxy_vec, pIyy_vec, pIxz_vec, pIyz_vec;
        v_float32x4 derivNorm_vec, derivNorm2_vec, weight_vec;
        v_float32x4 Ik1z_vec, Ik1zx_vec, Ik1zy_vec;
        v_float32x4 pa11_vec, pa12_vec, pa22_vec, pb1_vec, pb2_vec;

        for (; j < len - 3; j += 4)
        {
            pIx_vec = v_load(pIx + j);
            pIy_vec = v_load(pIy + j);
            pIz_vec = v_load(pIz + j);
            pdU_vec = v_load(pdU + j);
            pdV_vec = v_load(pdV + j);

            derivNorm_vec = pIx_vec * pIx_vec + pIy_vec * pIy_vec + zeta_vec;
            Ik1z_vec = pIz_vec + pIx_vec * pdU_vec + pIy_vec * pdV_vec;
            weight_vec = (delta_vec / v_sqrt(Ik1z_vec * Ik1z_vec / derivNorm_vec + eps_vec)) / derivNorm_vec;

            pa11_vec = weight_vec * (pIx_vec * pIx_vec) + zeta_vec;
            pa12_vec = weight_vec * (pIx_vec * pIy_vec);
            pa22_vec = weight_vec * (pIy_vec * pIy_vec) + zeta_vec;
            pb1_vec = zero_vec - weight_vec * (pIz_vec * pIx_vec);
            pb2_vec = zero_vec - weight_vec * (pIz_vec * pIy_vec);

            pIxx_vec = v_load(pIxx + j);
            pIxy_vec = v_load(pIxy + j);
            pIyy_vec = v_load(pIyy + j);
            pIxz_vec = v_load(pIxz + j);
            pIyz_vec = v_load(pIyz + j);

            derivNorm_vec = pIxx_vec * pIxx_vec + pIxy_vec * pIxy_vec + zeta_vec;
            derivNorm2_vec = pIyy_vec * pIyy_vec + pIxy_vec * pIxy_vec + zeta_vec;
            Ik1zx_vec = pIxz_vec + pIxx_vec * pdU_vec + pIxy_vec * pdV_vec;
            Ik1zy_vec = pIyz_vec + pIxy_vec * pdU_vec + pIyy_vec * pdV_vec;
            weight_vec = gamma_vec / v_sqrt(Ik1zx_vec * Ik1zx_vec / derivNorm_vec +
                                            Ik1zy_vec * Ik1zy_vec / derivNorm2_vec + eps_vec);

            pa11_vec += weight_vec * (pIxx_vec * pIxx_vec / derivNorm_vec + pIxy_vec * pIxy_vec / derivNorm2_vec);
            pa12_vec += weight_vec * (pIxx_vec * pIxy_vec / derivNorm_vec + pIxy_vec * pIyy_vec / derivNorm2_vec);
            pa22_vec += weight_vec * (pIxy_vec * pIxy_vec / derivNorm_vec + pIyy_vec * pIyy_vec / derivNorm2_vec);
            pb1_vec -= weight_vec * (pIxx_vec * pIxz_vec / derivNorm_vec + pIxy_vec * pIyz_vec / derivNorm2_vec);
            pb2_vec -= weight_vec * (pIxy_vec * pIxz_vec / derivNorm_vec + pIyy_vec * pIyz_vec / derivNorm2_vec);

            v_store(pa11 + j, pa11_vec);
            v_store(pa12 + j, pa12_vec);
            v_store(pa22 + j, pa22_vec);
            v_store(pb1 + j, pb1_vec);
            v_store(pb2 + j, pb2_vec);
        }
#endif
        for (; j < len; j++)
        {
            /* Step 1: Compute color constancy terms */
            /* Normalization factor:*/
            derivNorm = pIx[j] * pIx[j] + pIy[j] * pIy[j] + zeta_squared;
            /* Color constancy penalty (computed by Taylor expansion):*/
            Ik1z = pIz[j] + pIx[j] * pdU[j] + pIy[j] * pdV[j];
            /* Weight of the color constancy term in the current fixed-point iteration divided by derivNorm: */
            weight = (delta2 / sqrt(Ik1z * Ik1z / derivNorm + epsilon_squared)) / derivNorm;
            /* Add respective color constancy terms to the linear system coefficients: */
            pa11[j] = weight * (pIx[j] * pIx[j]) + zeta_squared;
            pa12[j] = weight * (pIx[j] * pIy[j]);
            pa22[j] = weight * (pIy[j] * pIy[j]) + zeta_squared;
            pb1[j] = -weight * (pIz[j] * pIx[j]);
            pb2[j] = -weight * (pIz[j] * pIy[j]);

            /* Step 2: Compute gradient constancy terms */
            /* Normalization factor for x gradient: */
            derivNorm = pIxx[j] * pIxx[j] + pIxy[j] * pIxy[j] + zeta_squared;
            /* Normalization factor for y gradient: */
            derivNorm2 = pIyy[j] * pIyy[j] + pIxy[j] * pIxy[j] + zeta_squared;
            /* Gradient constancy penalties (computed by Taylor expansion): */
            Ik1zx = pIxz[j] + pIxx[j] * pdU[j] + pIxy[j] * pdV[j];
            Ik1zy = pIyz[j] + pIxy[j] * pdU[j] + pIyy[j] * pdV[j];
            /* Weight of the gradient constancy term in the current fixed-point iteration: */
            weight = gamma2 / sqrt(Ik1zx * Ik1zx / derivNorm + Ik1zy * Ik1zy / derivNorm2 + epsilon_squared);
            /* Add respective gradient constancy components to the linear system coefficients: */
            pa11[j] += weight * (pIxx[j] * pIxx[j] / derivNorm + pIxy[j] * pIxy[j] / derivNorm2);
            pa12[j] += weight * (pIxx[j] * pIxy[j] / derivNorm + pIxy[j] * pIyy[j] / derivNorm2);
            pa22[j] += weight * (pIxy[j] * pIxy[j] / derivNorm + pIyy[j] * pIyy[j] / derivNorm2);
            pb1[j] += -weight * (pIxx[j] * pIxz[j] / derivNorm + pIxy[j] * pIyz[j] / derivNorm2);
            pb2[j] += -weight * (pIxy[j] * pIxz[j] / derivNorm + pIyy[j] * pIyz[j] / derivNorm2);
        }
    }
}