void VariationalRefinementImpl::RedBlackSOR_ParBody::operator()(const Range &range) const
{
    int start = min(range.start * stripe_sz, h);
    int end = min(range.end * stripe_sz, h);

    float *pa11, *pa12, *pa22, *pb1, *pb2, *pW, *pdu, *pdv;
    float *pW_next, *pdu_next, *pdv_next;
    float *pW_prev_row, *pdu_prev_row, *pdv_prev_row;
    float *pdu_next_row, *pdv_next_row;

    float sigmaU, sigmaV;
    int j, len;
    for (int i = start; i < end; i++)
    {
#define INIT_ROW_POINTERS(cur_color, next_color, next_offs_even, next_offs_odd)                                        \
    pW = var->weights.cur_color.ptr<float>(i + 1) + 1;                                                                 \
    pa11 = var->A11.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pa12 = var->A12.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pa22 = var->A22.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pb1 = var->b1.cur_color.ptr<float>(i + 1) + 1;                                                                     \
    pb2 = var->b2.cur_color.ptr<float>(i + 1) + 1;                                                                     \
    pdu = dW_u->cur_color.ptr<float>(i + 1) + 1;                                                                       \
    pdv = dW_v->cur_color.ptr<float>(i + 1) + 1;                                                                       \
                                                                                                                       \
    pdu_next_row = dW_u->next_color.ptr<float>(i + 2) + 1;                                                             \
    pdv_next_row = dW_v->next_color.ptr<float>(i + 2) + 1;                                                             \
                                                                                                                       \
    pW_prev_row = var->weights.next_color.ptr<float>(i) + 1;                                                           \
    pdu_prev_row = dW_u->next_color.ptr<float>(i) + 1;                                                                 \
    pdv_prev_row = dW_v->next_color.ptr<float>(i) + 1;                                                                 \
                                                                                                                       \
    if (i % 2 == 0)                                                                                                    \
    {                                                                                                                  \
        pW_next = var->weights.next_color.ptr<float>(i + 1) + next_offs_even;                                          \
        pdu_next = dW_u->next_color.ptr<float>(i + 1) + next_offs_even;                                                \
        pdv_next = dW_v->next_color.ptr<float>(i + 1) + next_offs_even;                                                \
        len = var->A11.cur_color##_even_len;                                                                           \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        pW_next = var->weights.next_color.ptr<float>(i + 1) + next_offs_odd;                                           \
        pdu_next = dW_u->next_color.ptr<float>(i + 1) + next_offs_odd;                                                 \
        pdv_next = dW_v->next_color.ptr<float>(i + 1) + next_offs_odd;                                                 \
        len = var->A11.cur_color##_odd_len;                                                                            \
    }
        if (red_pass)
        {
            INIT_ROW_POINTERS(red, black, 1, 2);
        }
        else
        {
            INIT_ROW_POINTERS(black, red, 2, 1);
        }
#undef INIT_ROW_POINTERS

        j = 0;
#if CV_SIMD128
        v_float32x4 pW_prev_vec = v_setall_f32(pW_next[-1]);
        v_float32x4 pdu_prev_vec = v_setall_f32(pdu_next[-1]);
        v_float32x4 pdv_prev_vec = v_setall_f32(pdv_next[-1]);
        v_float32x4 omega_vec = v_setall_f32(var->omega);
        v_float32x4 pW_vec, pW_next_vec, pW_prev_row_vec;
        v_float32x4 pdu_next_vec, pdu_prev_row_vec, pdu_next_row_vec;
        v_float32x4 pdv_next_vec, pdv_prev_row_vec, pdv_next_row_vec;
        v_float32x4 pW_shifted_vec, pdu_shifted_vec, pdv_shifted_vec;
        v_float32x4 pa12_vec, sigmaU_vec, sigmaV_vec, pdu_vec, pdv_vec;
        for (; j < len - 3; j += 4)
        {
            pW_vec = v_load(pW + j);
            pW_next_vec = v_load(pW_next + j);
            pW_prev_row_vec = v_load(pW_prev_row + j);
            pdu_next_vec = v_load(pdu_next + j);
            pdu_prev_row_vec = v_load(pdu_prev_row + j);
            pdu_next_row_vec = v_load(pdu_next_row + j);
            pdv_next_vec = v_load(pdv_next + j);
            pdv_prev_row_vec = v_load(pdv_prev_row + j);
            pdv_next_row_vec = v_load(pdv_next_row + j);
            pa12_vec = v_load(pa12 + j);
            pW_shifted_vec = v_reinterpret_as_f32(
              v_extract<3>(v_reinterpret_as_s32(pW_prev_vec), v_reinterpret_as_s32(pW_next_vec)));
            pdu_shifted_vec = v_reinterpret_as_f32(
              v_extract<3>(v_reinterpret_as_s32(pdu_prev_vec), v_reinterpret_as_s32(pdu_next_vec)));
            pdv_shifted_vec = v_reinterpret_as_f32(
              v_extract<3>(v_reinterpret_as_s32(pdv_prev_vec), v_reinterpret_as_s32(pdv_next_vec)));

            sigmaU_vec = pW_shifted_vec * pdu_shifted_vec + pW_vec * pdu_next_vec + pW_prev_row_vec * pdu_prev_row_vec +
                         pW_vec * pdu_next_row_vec;
            sigmaV_vec = pW_shifted_vec * pdv_shifted_vec + pW_vec * pdv_next_vec + pW_prev_row_vec * pdv_prev_row_vec +
                         pW_vec * pdv_next_row_vec;

            pdu_vec = v_load(pdu + j);
            pdv_vec = v_load(pdv + j);
            pdu_vec += omega_vec * ((sigmaU_vec + v_load(pb1 + j) - pdv_vec * pa12_vec) / v_load(pa11 + j) - pdu_vec);
            pdv_vec += omega_vec * ((sigmaV_vec + v_load(pb2 + j) - pdu_vec * pa12_vec) / v_load(pa22 + j) - pdv_vec);
            v_store(pdu + j, pdu_vec);
            v_store(pdv + j, pdv_vec);

            pW_prev_vec = pW_next_vec;
            pdu_prev_vec = pdu_next_vec;
            pdv_prev_vec = pdv_next_vec;
        }
#endif
        for (; j < len; j++)
        {
            sigmaU = pW_next[j - 1] * pdu_next[j - 1] + pW[j] * pdu_next[j] + pW_prev_row[j] * pdu_prev_row[j] +
                     pW[j] * pdu_next_row[j];
            sigmaV = pW_next[j - 1] * pdv_next[j - 1] + pW[j] * pdv_next[j] + pW_prev_row[j] * pdv_prev_row[j] +
                     pW[j] * pdv_next_row[j];
            pdu[j] += var->omega * ((sigmaU + pb1[j] - pdv[j] * pa12[j]) / pa11[j] - pdu[j]);
            pdv[j] += var->omega * ((sigmaV + pb2[j] - pdu[j] * pa12[j]) / pa22[j] - pdv[j]);
        }
    }
}