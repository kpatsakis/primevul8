 */
static int intel_hda_and_codec_init(PCIBus *bus)
{
    DeviceState *controller;
    BusState *hdabus;
    DeviceState *codec;

    warn_report("'-soundhw hda' is deprecated, "
                "please use '-device intel-hda -device hda-duplex' instead");
    controller = DEVICE(pci_create_simple(bus, -1, "intel-hda"));
    hdabus = QLIST_FIRST(&controller->child_bus);
    codec = qdev_new("hda-duplex");
    qdev_realize_and_unref(codec, hdabus, &error_fatal);
    return 0;