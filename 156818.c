static int32_t scsi_block_dma_command(SCSIRequest *req, uint8_t *buf)
{
    SCSIBlockReq *r = (SCSIBlockReq *)req;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, req->dev);

    r->cmd = req->cmd.buf[0];
    switch (r->cmd >> 5) {
    case 0:
        /* 6-byte CDB.  */
        r->cdb1 = r->group_number = 0;
        break;
    case 1:
        /* 10-byte CDB.  */
        r->cdb1 = req->cmd.buf[1];
        r->group_number = req->cmd.buf[6];
        break;
    case 4:
        /* 12-byte CDB.  */
        r->cdb1 = req->cmd.buf[1];
        r->group_number = req->cmd.buf[10];
        break;
    case 5:
        /* 16-byte CDB.  */
        r->cdb1 = req->cmd.buf[1];
        r->group_number = req->cmd.buf[14];
        break;
    default:
        abort();
    }

    /* Protection information is not supported.  For SCSI versions 2 and
     * older (as determined by snooping the guest's INQUIRY commands),
     * there is no RD/WR/VRPROTECT, so skip this check in these versions.
     */
    if (s->qdev.scsi_version > 2 && (req->cmd.buf[1] & 0xe0)) {
        scsi_check_condition(&r->req, SENSE_CODE(INVALID_FIELD));
        return 0;
    }

    return scsi_disk_dma_command(req, buf);
}