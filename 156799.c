static void scsi_disk_base_class_initfn(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    SCSIDiskClass *sdc = SCSI_DISK_BASE_CLASS(klass);

    dc->fw_name = "disk";
    dc->reset = scsi_disk_reset;
    sdc->dma_readv = scsi_dma_readv;
    sdc->dma_writev = scsi_dma_writev;
    sdc->need_fua_emulation = scsi_is_cmd_fua;
}