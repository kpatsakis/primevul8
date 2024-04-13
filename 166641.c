dict_threshold2_params(const ref * pdict, gs_threshold2_halftone * ptp,
                       ref * ptproc, gs_memory_t *mem)
{
    ref *tstring;
    int code =
        dict_threshold_common_params(pdict,
                                     (gs_threshold_halftone_common *)ptp,
                                     &tstring, ptproc);
    int bps;
    uint size;
    int cw2, ch2;

    if (code < 0 ||
        (code = cw2 = dict_int_param(pdict, "Width2", 0, 0x7fff, 0,
                                     &ptp->width2)) < 0 ||
        (code = ch2 = dict_int_param(pdict, "Height2", 0, 0x7fff, 0,
                                     &ptp->height2)) < 0 ||
        (code = dict_int_param(pdict, "BitsPerSample", 8, 16, -1, &bps)) < 0
        )
        return code;
    if ((bps != 8 && bps != 16) || cw2 != ch2 ||
        (!cw2 && (ptp->width2 == 0 || ptp->height2 == 0))
        )
        return_error(gs_error_rangecheck);
    ptp->bytes_per_sample = bps / 8;
    switch (r_type(tstring)) {
    case t_string:
        size = r_size(tstring);
        gs_bytestring_from_string(&ptp->thresholds, tstring->value.const_bytes,
                                  size);
        break;
    case t_astruct:
        if (gs_object_type(mem, tstring->value.pstruct) != &st_bytes)
            return_error(gs_error_typecheck);
        size = gs_object_size(mem, tstring->value.pstruct);
        gs_bytestring_from_bytes(&ptp->thresholds, r_ptr(tstring, byte),
                                 0, size);
        break;
    default:
        return_error(gs_error_typecheck);
    }
    check_read(*tstring);
    if (size != (ptp->width * ptp->height + ptp->width2 * ptp->height2) *
        ptp->bytes_per_sample)
        return_error(gs_error_rangecheck);
    return 0;
}