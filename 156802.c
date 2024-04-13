static void scsi_hd_class_initfn(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SCSIDeviceClass *sc = SCSI_DEVICE_CLASS(klass);

    sc->realize      = scsi_hd_realize;
    sc->unrealize    = scsi_unrealize;
    sc->alloc_req    = scsi_new_request;
    sc->unit_attention_reported = scsi_disk_unit_attention_reported;
    dc->desc = "virtual SCSI disk";
    device_class_set_props(dc, scsi_hd_properties);
    dc->vmsd  = &vmstate_scsi_disk_state;
}