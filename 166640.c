dict_threshold_common_params(const ref * pdict,
                             gs_threshold_halftone_common * ptp,
                             ref **pptstring, ref *ptproc)
{
    int code;

    check_dict_read(*pdict);
    if ((code = dict_int_param(pdict, "Width", 1, 0x7fff, -1,
                               &ptp->width)) < 0 ||
        (code = dict_int_param(pdict, "Height", 1, 0x7fff, -1,
                               &ptp->height)) < 0 ||
        (code = dict_find_string(pdict, "Thresholds", pptstring)) <= 0 ||
      (code = dict_proc_param(pdict, "TransferFunction", ptproc, false)) < 0
        )
        return (code < 0 ? code : gs_error_undefined);
    ptp->transfer_closure.proc = 0;
    ptp->transfer_closure.data = 0;
    return code;
}