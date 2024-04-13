static bool scsi_cd_is_medium_locked(void *opaque)
{
    return ((SCSIDiskState *)opaque)->tray_locked;
}