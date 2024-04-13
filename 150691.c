void DISOpticalFlowImpl::Densification_ParBody::operator()(const Range &range) const
{
    int start_i = min(range.start * stripe_sz, h);
    int end_i = min(range.end * stripe_sz, h);

    /* Input sparse flow */
    float *Sx_ptr = Sx->ptr<float>();
    float *Sy_ptr = Sy->ptr<float>();

    /* Output dense flow */
    float *Ux_ptr = Ux->ptr<float>();
    float *Uy_ptr = Uy->ptr<float>();

    uchar *I0_ptr = I0->ptr<uchar>();
    uchar *I1_ptr = I1->ptr<uchar>();

    int psz = dis->patch_size;
    int pstr = dis->patch_stride;
    int i_l, i_u;
    int j_l, j_u;
    float i_m, j_m, diff;

    /* These values define the set of sparse grid locations that contain patches overlapping with the current dense flow
     * location */
    int start_is, end_is;
    int start_js, end_js;

/* Some helper macros for updating this set of sparse grid locations */
#define UPDATE_SPARSE_I_COORDINATES                                                                                    \
    if (i % pstr == 0 && i + psz <= h)                                                                                 \
        end_is++;                                                                                                      \
    if (i - psz >= 0 && (i - psz) % pstr == 0 && start_is < end_is)                                                    \
        start_is++;

#define UPDATE_SPARSE_J_COORDINATES                                                                                    \
    if (j % pstr == 0 && j + psz <= dis->w)                                                                            \
        end_js++;                                                                                                      \
    if (j - psz >= 0 && (j - psz) % pstr == 0 && start_js < end_js)                                                    \
        start_js++;

    start_is = 0;
    end_is = -1;
    for (int i = 0; i < start_i; i++)
    {
        UPDATE_SPARSE_I_COORDINATES;
    }
    for (int i = start_i; i < end_i; i++)
    {
        UPDATE_SPARSE_I_COORDINATES;
        start_js = 0;
        end_js = -1;
        for (int j = 0; j < dis->w; j++)
        {
            UPDATE_SPARSE_J_COORDINATES;
            float coef, sum_coef = 0.0f;
            float sum_Ux = 0.0f;
            float sum_Uy = 0.0f;

            /* Iterate through all the patches that overlap the current location (i,j) */
            for (int is = start_is; is <= end_is; is++)
                for (int js = start_js; js <= end_js; js++)
                {
                    j_m = min(max(j + Sx_ptr[is * dis->ws + js], 0.0f), dis->w - 1.0f - EPS);
                    i_m = min(max(i + Sy_ptr[is * dis->ws + js], 0.0f), dis->h - 1.0f - EPS);
                    j_l = (int)j_m;
                    j_u = j_l + 1;
                    i_l = (int)i_m;
                    i_u = i_l + 1;
                    diff = (j_m - j_l) * (i_m - i_l) * I1_ptr[i_u * dis->w + j_u] +
                           (j_u - j_m) * (i_m - i_l) * I1_ptr[i_u * dis->w + j_l] +
                           (j_m - j_l) * (i_u - i_m) * I1_ptr[i_l * dis->w + j_u] +
                           (j_u - j_m) * (i_u - i_m) * I1_ptr[i_l * dis->w + j_l] - I0_ptr[i * dis->w + j];
                    coef = 1 / max(1.0f, abs(diff));
                    sum_Ux += coef * Sx_ptr[is * dis->ws + js];
                    sum_Uy += coef * Sy_ptr[is * dis->ws + js];
                    sum_coef += coef;
                }
            Ux_ptr[i * dis->w + j] = sum_Ux / sum_coef;
            Uy_ptr[i * dis->w + j] = sum_Uy / sum_coef;
        }
    }
#undef UPDATE_SPARSE_I_COORDINATES
#undef UPDATE_SPARSE_J_COORDINATES
}