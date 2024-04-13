void VariationalRefinementImpl::ComputeSmoothnessTermHorPass_ParBody::operator()(const Range &range) const
{
    int start_i = min(range.start * stripe_sz, h);
    int end_i = min(range.end * stripe_sz, h);

    float epsilon_squared = var->epsilon * var->epsilon;
    float alpha2 = var->alpha / 2;
    float *pWeight;
    float *pA_u, *pA_u_next, *pA_v, *pA_v_next;
    float *pB_u, *pB_u_next, *pB_v, *pB_v_next;
    float *cW_u, *cW_u_next, *cW_u_next_row;
    float *cW_v, *cW_v_next, *cW_v_next_row;
    float *pW_u, *pW_u_next;
    float *pW_v, *pW_v_next;
    float ux, uy, vx, vy;
    int len;
    bool touches_right_border = true;

#define INIT_ROW_POINTERS(cur_color, next_color, next_offs_even, next_offs_odd, bool_default)                          \
    pWeight = var->weights.cur_color.ptr<float>(i + 1) + 1;                                                            \
    pA_u = var->A11.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pB_u = var->b1.cur_color.ptr<float>(i + 1) + 1;                                                                    \
    cW_u = curW_u->cur_color.ptr<float>(i + 1) + 1;                                                                    \
    pW_u = W_u->cur_color.ptr<float>(i + 1) + 1;                                                                       \
    pA_v = var->A22.cur_color.ptr<float>(i + 1) + 1;                                                                   \
    pB_v = var->b2.cur_color.ptr<float>(i + 1) + 1;                                                                    \
    cW_v = curW_v->cur_color.ptr<float>(i + 1) + 1;                                                                    \
    pW_v = W_v->cur_color.ptr<float>(i + 1) + 1;                                                                       \
                                                                                                                       \
    cW_u_next_row = curW_u->next_color.ptr<float>(i + 2) + 1;                                                          \
    cW_v_next_row = curW_v->next_color.ptr<float>(i + 2) + 1;                                                          \
                                                                                                                       \
    if (i % 2 == 0)                                                                                                    \
    {                                                                                                                  \
        pA_u_next = var->A11.next_color.ptr<float>(i + 1) + next_offs_even;                                            \
        pB_u_next = var->b1.next_color.ptr<float>(i + 1) + next_offs_even;                                             \
        cW_u_next = curW_u->next_color.ptr<float>(i + 1) + next_offs_even;                                             \
        pW_u_next = W_u->next_color.ptr<float>(i + 1) + next_offs_even;                                                \
        pA_v_next = var->A22.next_color.ptr<float>(i + 1) + next_offs_even;                                            \
        pB_v_next = var->b2.next_color.ptr<float>(i + 1) + next_offs_even;                                             \
        cW_v_next = curW_v->next_color.ptr<float>(i + 1) + next_offs_even;                                             \
        pW_v_next = W_v->next_color.ptr<float>(i + 1) + next_offs_even;                                                \
        len = var->A11.cur_color##_even_len;                                                                           \
        if (var->A11.cur_color##_even_len != var->A11.cur_color##_odd_len)                                             \
            touches_right_border = bool_default;                                                                       \
        else                                                                                                           \
            touches_right_border = !bool_default;                                                                      \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        pA_u_next = var->A11.next_color.ptr<float>(i + 1) + next_offs_odd;                                             \
        pB_u_next = var->b1.next_color.ptr<float>(i + 1) + next_offs_odd;                                              \
        cW_u_next = curW_u->next_color.ptr<float>(i + 1) + next_offs_odd;                                              \
        pW_u_next = W_u->next_color.ptr<float>(i + 1) + next_offs_odd;                                                 \
        pA_v_next = var->A22.next_color.ptr<float>(i + 1) + next_offs_odd;                                             \
        pB_v_next = var->b2.next_color.ptr<float>(i + 1) + next_offs_odd;                                              \
        cW_v_next = curW_v->next_color.ptr<float>(i + 1) + next_offs_odd;                                              \
        pW_v_next = W_v->next_color.ptr<float>(i + 1) + next_offs_odd;                                                 \
        len = var->A11.cur_color##_odd_len;                                                                            \
        if (var->A11.cur_color##_even_len != var->A11.cur_color##_odd_len)                                             \
            touches_right_border = !bool_default;                                                                      \
        else                                                                                                           \
            touches_right_border = bool_default;                                                                       \
    }

    for (int i = start_i; i < end_i; i++)
    {
        if (red_pass)
        {
            INIT_ROW_POINTERS(red, black, 1, 2, true);
        }
        else
        {
            INIT_ROW_POINTERS(black, red, 2, 1, false);
        }
#undef INIT_ROW_POINTERS

#define COMPUTE                                                                                                        \
    /* Gradients for the flow on the current fixed-point iteration: */                                                 \
    ux = cW_u_next[j] - cW_u[j];                                                                                       \
    vx = cW_v_next[j] - cW_v[j];                                                                                       \
    uy = cW_u_next_row[j] - cW_u[j];                                                                                   \
    vy = cW_v_next_row[j] - cW_v[j];                                                                                   \
    /* Weight of the smoothness term in the current fixed-point iteration: */                                          \
    pWeight[j] = alpha2 / sqrt(ux * ux + vx * vx + uy * uy + vy * vy + epsilon_squared);                               \
    /* Gradients for initial raw flow multiplied by weight:*/                                                          \
    ux = pWeight[j] * (pW_u_next[j] - pW_u[j]);                                                                        \
    vx = pWeight[j] * (pW_v_next[j] - pW_v[j]);

#define UPDATE                                                                                                         \
    pB_u[j] += ux;                                                                                                     \
    pA_u[j] += pWeight[j];                                                                                             \
    pB_v[j] += vx;                                                                                                     \
    pA_v[j] += pWeight[j];                                                                                             \
    pB_u_next[j] -= ux;                                                                                                \
    pA_u_next[j] += pWeight[j];                                                                                        \
    pB_v_next[j] -= vx;                                                                                                \
    pA_v_next[j] += pWeight[j];

        int j = 0;
#if CV_SIMD128
        v_float32x4 alpha2_vec = v_setall_f32(alpha2);
        v_float32x4 eps_vec = v_setall_f32(epsilon_squared);
        v_float32x4 cW_u_vec, cW_v_vec;
        v_float32x4 pWeight_vec, ux_vec, vx_vec, uy_vec, vy_vec;

        for (; j < len - 4; j += 4)
        {
            cW_u_vec = v_load(cW_u + j);
            cW_v_vec = v_load(cW_v + j);

            ux_vec = v_load(cW_u_next + j) - cW_u_vec;
            vx_vec = v_load(cW_v_next + j) - cW_v_vec;
            uy_vec = v_load(cW_u_next_row + j) - cW_u_vec;
            vy_vec = v_load(cW_v_next_row + j) - cW_v_vec;
            pWeight_vec =
              alpha2_vec / v_sqrt(ux_vec * ux_vec + vx_vec * vx_vec + uy_vec * uy_vec + vy_vec * vy_vec + eps_vec);
            v_store(pWeight + j, pWeight_vec);

            ux_vec = pWeight_vec * (v_load(pW_u_next + j) - v_load(pW_u + j));
            vx_vec = pWeight_vec * (v_load(pW_v_next + j) - v_load(pW_v + j));

            v_store(pA_u + j, v_load(pA_u + j) + pWeight_vec);
            v_store(pA_v + j, v_load(pA_v + j) + pWeight_vec);
            v_store(pB_u + j, v_load(pB_u + j) + ux_vec);
            v_store(pB_v + j, v_load(pB_v + j) + vx_vec);

            v_store(pA_u_next + j, v_load(pA_u_next + j) + pWeight_vec);
            v_store(pA_v_next + j, v_load(pA_v_next + j) + pWeight_vec);
            v_store(pB_u_next + j, v_load(pB_u_next + j) - ux_vec);
            v_store(pB_v_next + j, v_load(pB_v_next + j) - vx_vec);
        }
#endif
        for (; j < len - 1; j++)
        {
            COMPUTE;
            UPDATE;
        }

        /* Omit the update on the rightmost elements */
        if (touches_right_border)
        {
            COMPUTE;
        }
        else
        {
            COMPUTE;
            UPDATE;
        }
    }
#undef COMPUTE
#undef UPDATE
}