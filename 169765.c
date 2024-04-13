
static void intel_hda_realize(PCIDevice *pci, Error **errp)
{
    IntelHDAState *d = INTEL_HDA(pci);
    uint8_t *conf = d->pci.config;
    Error *err = NULL;
    int ret;

    d->name = object_get_typename(OBJECT(d));

    pci_config_set_interrupt_pin(conf, 1);

    /* HDCTL off 0x40 bit 0 selects signaling mode (1-HDA, 0 - Ac97) 18.1.19 */
    conf[0x40] = 0x01;

    if (d->msi != ON_OFF_AUTO_OFF) {
        ret = msi_init(&d->pci, d->old_msi_addr ? 0x50 : 0x60,
                       1, true, false, &err);
        /* Any error other than -ENOTSUP(board's MSI support is broken)
         * is a programming error */
        assert(!ret || ret == -ENOTSUP);
        if (ret && d->msi == ON_OFF_AUTO_ON) {
            /* Can't satisfy user's explicit msi=on request, fail */
            error_append_hint(&err, "You have to use msi=auto (default) or "
                    "msi=off with this machine type.\n");
            error_propagate(errp, err);
            return;
        }
        assert(!err || d->msi == ON_OFF_AUTO_AUTO);
        /* With msi=auto, we fall back to MSI off silently */
        error_free(err);
    }

    memory_region_init(&d->container, OBJECT(d),
                       "intel-hda-container", 0x4000);
    memory_region_init_io(&d->mmio, OBJECT(d), &intel_hda_mmio_ops, d,
                          "intel-hda", 0x2000);
    memory_region_add_subregion(&d->container, 0x0000, &d->mmio);
    memory_region_init_alias(&d->alias, OBJECT(d), "intel-hda-alias",
                             &d->mmio, 0, 0x2000);
    memory_region_add_subregion(&d->container, 0x2000, &d->alias);
    pci_register_bar(&d->pci, 0, 0, &d->container);

    hda_codec_bus_init(DEVICE(pci), &d->codecs, sizeof(d->codecs),
                       intel_hda_response, intel_hda_xfer);