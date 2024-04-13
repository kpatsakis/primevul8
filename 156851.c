static void scsi_disk_register_types(void)
{
    type_register_static(&scsi_disk_base_info);
    type_register_static(&scsi_hd_info);
    type_register_static(&scsi_cd_info);
#ifdef __linux__
    type_register_static(&scsi_block_info);
#endif
}