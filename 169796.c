
static void intel_hda_reset(DeviceState *dev)
{
    BusChild *kid;
    IntelHDAState *d = INTEL_HDA(dev);
    HDACodecDevice *cdev;

    intel_hda_regs_reset(d);
    d->wall_base_ns = qemu_clock_get_ns(QEMU_CLOCK_VIRTUAL);

    /* reset codecs */
    QTAILQ_FOREACH(kid, &d->codecs.qbus.children, sibling) {
        DeviceState *qdev = kid->child;
        cdev = HDA_CODEC_DEVICE(qdev);
        device_legacy_reset(DEVICE(cdev));
        d->state_sts |= (1 << cdev->cad);
    }
    intel_hda_update_irq(d);