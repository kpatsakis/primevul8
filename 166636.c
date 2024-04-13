sethalftone_cleanup(i_ctx_t *i_ctx_p)
{
    gx_device_halftone *pdht = r_ptr(&esp[4], gx_device_halftone);
    gs_halftone *pht = r_ptr(&esp[3], gs_halftone);

    gs_free_object(pdht->rc.memory, pdht,
                   "sethalftone_cleanup(device halftone)");
    gs_free_object(pht->rc.memory, pht,
                   "sethalftone_cleanup(halftone)");
    return 0;
}