static void scsi_block_class_initfn(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SCSIDeviceClass *sc = SCSI_DEVICE_CLASS(klass);
    SCSIDiskClass *sdc = SCSI_DISK_BASE_CLASS(klass);

    sc->realize      = scsi_block_realize;
    sc->alloc_req    = scsi_block_new_request;
    sc->parse_cdb    = scsi_block_parse_cdb;
    sdc->dma_readv   = scsi_block_dma_readv;
    sdc->dma_writev  = scsi_block_dma_writev;
    sdc->update_sense = scsi_block_update_sense;
    sdc->need_fua_emulation = scsi_block_no_fua;
    dc->desc = "SCSI block device passthrough";
    device_class_set_props(dc, scsi_block_properties);
    dc->vmsd  = &vmstate_scsi_disk_state;
}