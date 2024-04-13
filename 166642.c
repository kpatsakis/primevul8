dict_threshold_params(const ref * pdict, gs_threshold_halftone * ptp,
                      ref * ptproc)
{
    ref *tstring;
    int code =
        dict_threshold_common_params(pdict,
                                     (gs_threshold_halftone_common *)ptp,
                                     &tstring, ptproc);

    if (code < 0)
        return code;
    check_read_type_only(*tstring, t_string);
    if (r_size(tstring) != (long)ptp->width * ptp->height)
        return_error(gs_error_rangecheck);
    ptp->thresholds.data = tstring->value.const_bytes;
    ptp->thresholds.size = r_size(tstring);
    ptp->transfer = (code > 0 ? (gs_mapping_proc) 0 : gs_mapped_transfer);
    return 0;
}