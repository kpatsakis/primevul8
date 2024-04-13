static bool scsi_cd_is_tray_open(void *opaque)
{
    return ((SCSIDiskState *)opaque)->tray_open;
}