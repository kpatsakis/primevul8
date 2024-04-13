
static void intel_hda_register_types(void)
{
    type_register_static(&hda_codec_bus_info);
    type_register_static(&intel_hda_info);
    type_register_static(&intel_hda_info_ich6);
    type_register_static(&intel_hda_info_ich9);
    type_register_static(&hda_codec_device_type_info);
    pci_register_soundhw("hda", "Intel HD Audio", intel_hda_and_codec_init);