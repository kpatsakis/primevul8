static void scsi_unrealize(SCSIDevice *dev)
{
    del_boot_device_lchs(&dev->qdev, NULL);
}