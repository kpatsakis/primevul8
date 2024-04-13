
static void hda_codec_device_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *k = DEVICE_CLASS(klass);
    k->realize = hda_codec_dev_realize;
    k->unrealize = hda_codec_dev_unrealize;
    set_bit(DEVICE_CATEGORY_SOUND, k->categories);
    k->bus_type = TYPE_HDA_BUS;
    device_class_set_props(k, hda_props);