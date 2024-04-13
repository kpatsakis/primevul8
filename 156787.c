static int scsi_block_parse_cdb(SCSIDevice *d, SCSICommand *cmd,
                                  uint8_t *buf, void *hba_private)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, d);

    if (scsi_block_is_passthrough(s, buf)) {
        return scsi_bus_parse_cdb(&s->qdev, cmd, buf, hba_private);
    } else {
        return scsi_req_parse_cdb(&s->qdev, cmd, buf);
    }
}