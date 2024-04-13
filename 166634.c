dict_spot_results(i_ctx_t *i_ctx_p, ref * pdict, const gs_spot_halftone * psp)
{
    int code;

    code = dict_real_result(i_ctx_p, pdict, "ActualFrequency",
                            psp->screen.actual_frequency);
    if (code < 0)
        return code;
    return dict_real_result(i_ctx_p, pdict, "ActualAngle",
                            psp->screen.actual_angle);
}