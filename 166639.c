sethalftone_finish(i_ctx_t *i_ctx_p)
{
    gx_device_halftone *pdht = r_ptr(esp, gx_device_halftone);
    int code;

    if (pdht->components)
        pdht->order = pdht->components[0].corder;
    code = gx_ht_install(igs, r_ptr(esp - 1, gs_halftone), pdht);
    if (code < 0)
        return code;
    istate->halftone = esp[-2];
    esp -= 4;
    sethalftone_cleanup(i_ctx_p);
    return o_pop_estack;
}