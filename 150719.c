void VariationalRefinementImpl::ComputeSmoothnessTermVertPass_ParBody::operator()(const Range &range) const
{
    int start_i = min(range.start * stripe_sz, h);
    int end_i = min(range.end * stripe_sz, h);

    float *pWeight;
    float *pA_u, *pA_u_next_row, *pA_v, *pA_v_next_row;
    float *pB_u, *pB_u_next_row, *pB_v, *pB_v_next_row;
    float *pW_u, *pW_u_next_row, *pW_v, *pW_v_next_row;
    float vy, uy;
    int len;

    for (int i = start_i; i < end_i; i++)
    {
#define INIT_ROW_POINTERS(cur_color, next_color)                                                                       \
    pWeight = var->weights.cur_color.ptr<float>(i + 1) + 1;                                                            \
    pA_u = var->A11.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pB_u = var->b1.cur_color.ptr<float>(i + 1) + 1;                                                                    \
    pW_u = W_u->cur_color.ptr<float>(i + 1) + 1;                                                                       \
    pA_v = var->A22.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pB_v = var->b2.cur_color.ptr<float>(i + 1) + 1;                                                                    \
    pW_v = W_v->cur_color.ptr<float>(i + 1) + 1;                                                                       \
                                                                                                                       \
    pA_u_next_row = var->A11.next_color.ptr<float>(i + 2) + 1;                                                         \
    pB_u_next_row = var->b1.next_color.ptr<float>(i + 2) + 1;                                                          \
    pW_u_next_row = W_u->next_color.ptr<float>(i + 2) + 1;                                                             \
    pA_v_next_row = var->A22.next_color.ptr<float>(i + 2) + 1;                                                         \
    pB_v_next_row = var->b2.next_color.ptr<float>(i + 2) + 1;                                                          \
    pW_v_next_row = W_v->next_color.ptr<float>(i + 2) + 1;                                                             \
                                                                                                                       \
    if (i % 2 == 0)                                                                                                    \
        len = var->A11.cur_color##_even_len;                                                                           \
    else                                                                                                               \
        len = var->A11.cur_color##_odd_len;

        if (red_pass)
        {
            INIT_ROW_POINTERS(red, black);
        }
        else
        {
            INIT_ROW_POINTERS(black, red);
        }
#undef INIT_ROW_POINTERS

        int j = 0;
#if CV_SIMD128
        v_float32x4 pWeight_vec, uy_vec, vy_vec;
        for (; j < len - 3; j += 4)
        {
            pWeight_vec = v_load(pWeight + j);
            uy_vec = pWeight_vec * (v_load(pW_u_next_row + j) - v_load(pW_u + j));
            vy_vec = pWeight_vec * (v_load(pW_v_next_row + j) - v_load(pW_v + j));

            v_store(pA_u + j, v_load(pA_u + j) + pWeight_vec);
            v_store(pA_v + j, v_load(pA_v + j) + pWeight_vec);
            v_store(pB_u + j, v_load(pB_u + j) + uy_vec);
            v_store(pB_v + j, v_load(pB_v + j) + vy_vec);

            v_store(pA_u_next_row + j, v_load(pA_u_next_row + j) + pWeight_vec);
            v_store(pA_v_next_row + j, v_load(pA_v_next_row + j) + pWeight_vec);
            v_store(pB_u_next_row + j, v_load(pB_u_next_row + j) - uy_vec);
            v_store(pB_v_next_row + j, v_load(pB_v_next_row + j) - vy_vec);
        }
#endif
        for (; j < len; j++)
        {
            uy = pWeight[j] * (pW_u_next_row[j] - pW_u[j]);
            vy = pWeight[j] * (pW_v_next_row[j] - pW_v[j]);
            pB_u[j] += uy;
            pA_u[j] += pWeight[j];
            pB_v[j] += vy;
            pA_v[j] += pWeight[j];
            pB_u_next_row[j] -= uy;
            pA_u_next_row[j] += pWeight[j];
            pB_v_next_row[j] -= vy;
            pA_v_next_row[j] += pWeight[j];
        }
    }
}