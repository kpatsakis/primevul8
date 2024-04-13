
static void intel_hda_class_init_ich9(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(klass);

    k->device_id = 0x293e;
    k->revision = 3;
    set_bit(DEVICE_CATEGORY_SOUND, dc->categories);
    dc->desc = "Intel HD Audio Controller (ich9)";