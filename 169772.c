
static void intel_hda_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->realize = intel_hda_realize;
    k->exit = intel_hda_exit;
    k->vendor_id = PCI_VENDOR_ID_INTEL;
    k->class_id = PCI_CLASS_MULTIMEDIA_HD_AUDIO;
    dc->reset = intel_hda_reset;
    dc->vmsd = &vmstate_intel_hda;
    device_class_set_props(dc, intel_hda_properties);