gs_setdevice_no_erase(gs_gstate * pgs, gx_device * dev)
{
    int open_code = 0, code;
    gs_lib_ctx_t *libctx = gs_lib_ctx_get_interp_instance(pgs->memory);

    /* If the ICC manager is not yet initialized, set it up now.  But only
       if we have file io capability now */
    if (libctx->io_device_table != NULL) {
        cmm_dev_profile_t *dev_profile;
        if (pgs->icc_manager->lab_profile == NULL) {  /* pick one not set externally */
            gsicc_init_iccmanager(pgs);
        }
        /* Also, if the device profile is not yet set then take care of that
           before we start filling pages, if we can */
        /* Although device methods should not be NULL, they are not completely filled in until
         * gx_device_fill_in_procs is called, and its possible for us to get here before this
         * happens, so we *must* make sure the method is not NULL before we use it.
         */
        if (dev->procs.get_profile != NULL) {
            code = dev_proc(dev, get_profile)(dev, &dev_profile);
            if (code < 0) {
                return(code);
            }
            if (dev_profile == NULL ||
                dev_profile->device_profile[gsDEFAULTPROFILE] == NULL) {
                if ((code = gsicc_init_device_profile_struct(dev, NULL,
                                                        gsDEFAULTPROFILE)) < 0)
                    return(code);
                /* set the intent too */
                if ((code = gsicc_set_device_profile_intent(dev, gsRINOTSPECIFIED,
                                                       gsDEFAULTPROFILE)) < 0)
                    return(code);
            }
        }
    }

    /* Initialize the device */
    if (!dev->is_open) {
        gx_device_fill_in_procs(dev);

        /* If we have not yet done so, and if we can, set the device profile
         * Doing so *before* the device is opened means that a device which
         * opens other devices can pass a profile on - for example, pswrite
         * also opens a bbox device
         */
        if (libctx->io_device_table != NULL) {
            cmm_dev_profile_t *dev_profile;
            /* Although device methods should not be NULL, they are not completely filled in until
             * gx_device_fill_in_procs is called, and its possible for us to get here before this
             * happens, so we *must* make sure the method is not NULL before we use it.
             */
            if (dev->procs.get_profile != NULL) {
                code = dev_proc(dev, get_profile)(dev, &dev_profile);
                if (code < 0) {
                    return(code);
                }
                if (dev_profile == NULL ||
                    dev_profile->device_profile[gsDEFAULTPROFILE] == NULL) {
                    if ((code = gsicc_init_device_profile_struct(dev, NULL,
                                                            gsDEFAULTPROFILE)) < 0)
                        return(code);
                }
            }
        }

        if (gs_device_is_memory(dev)) {
            /* Set the target to the current device. */
            gx_device *odev = gs_currentdevice_inline(pgs);

            while (odev != 0 && gs_device_is_memory(odev))
                odev = ((gx_device_memory *)odev)->target;
            gx_device_set_target(((gx_device_forward *)dev), odev);
        }
        code = open_code = gs_opendevice(dev);
        if (code < 0)
            return code;
    }
    gs_setdevice_no_init(pgs, dev);
    pgs->ctm_default_set = false;
    if ((code = gs_initmatrix(pgs)) < 0 ||
        (code = gs_initclip(pgs)) < 0
        )
        return code;
    /* If we were in a charpath or a setcachedevice, */
    /* we aren't any longer. */
    pgs->in_cachedevice = 0;
    pgs->in_charpath = (gs_char_path_mode) 0;
    return open_code;
}