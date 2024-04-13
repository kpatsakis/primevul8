dict_spot_params(const ref * pdict, gs_spot_halftone * psp,
                 ref * psproc, ref * ptproc, gs_memory_t *mem)
{
    int code;

    check_dict_read(*pdict);
    if ((code = dict_float_param(pdict, "Frequency", 0.0,
                                 &psp->screen.frequency)) != 0 ||
        (code = dict_float_param(pdict, "Angle", 0.0,
                                 &psp->screen.angle)) != 0 ||
      (code = dict_proc_param(pdict, "SpotFunction", psproc, false)) != 0 ||
        (code = dict_bool_param(pdict, "AccurateScreens",
                                gs_currentaccuratescreens(mem),
                                &psp->accurate_screens)) < 0 ||
      (code = dict_proc_param(pdict, "TransferFunction", ptproc, false)) < 0
        )
        return (code < 0 ? code : gs_error_undefined);
    psp->transfer = (code > 0 ? (gs_mapping_proc) 0 : gs_mapped_transfer);
    psp->transfer_closure.proc = 0;
    psp->transfer_closure.data = 0;
    return 0;
}