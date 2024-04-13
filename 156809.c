static int32_t scsi_disk_dma_command(SCSIRequest *req, uint8_t *buf)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, req->dev);
    SCSIDiskClass *sdc = (SCSIDiskClass *) object_get_class(OBJECT(s));
    uint32_t len;
    uint8_t command;

    command = buf[0];

    if (!blk_is_available(s->qdev.conf.blk)) {
        scsi_check_condition(r, SENSE_CODE(NO_MEDIUM));
        return 0;
    }

    len = scsi_data_cdb_xfer(r->req.cmd.buf);
    switch (command) {
    case READ_6:
    case READ_10:
    case READ_12:
    case READ_16:
        trace_scsi_disk_dma_command_READ(r->req.cmd.lba, len);
        /* Protection information is not supported.  For SCSI versions 2 and
         * older (as determined by snooping the guest's INQUIRY commands),
         * there is no RD/WR/VRPROTECT, so skip this check in these versions.
         */
        if (s->qdev.scsi_version > 2 && (r->req.cmd.buf[1] & 0xe0)) {
            goto illegal_request;
        }
        if (!check_lba_range(s, r->req.cmd.lba, len)) {
            goto illegal_lba;
        }
        r->sector = r->req.cmd.lba * (s->qdev.blocksize / BDRV_SECTOR_SIZE);
        r->sector_count = len * (s->qdev.blocksize / BDRV_SECTOR_SIZE);
        break;
    case WRITE_6:
    case WRITE_10:
    case WRITE_12:
    case WRITE_16:
    case WRITE_VERIFY_10:
    case WRITE_VERIFY_12:
    case WRITE_VERIFY_16:
        if (!blk_is_writable(s->qdev.conf.blk)) {
            scsi_check_condition(r, SENSE_CODE(WRITE_PROTECTED));
            return 0;
        }
        trace_scsi_disk_dma_command_WRITE(
                (command & 0xe) == 0xe ? "And Verify " : "",
                r->req.cmd.lba, len);
        /* fall through */
    case VERIFY_10:
    case VERIFY_12:
    case VERIFY_16:
        /* We get here only for BYTCHK == 0x01 and only for scsi-block.
         * As far as DMA is concerned, we can treat it the same as a write;
         * scsi_block_do_sgio will send VERIFY commands.
         */
        if (s->qdev.scsi_version > 2 && (r->req.cmd.buf[1] & 0xe0)) {
            goto illegal_request;
        }
        if (!check_lba_range(s, r->req.cmd.lba, len)) {
            goto illegal_lba;
        }
        r->sector = r->req.cmd.lba * (s->qdev.blocksize / BDRV_SECTOR_SIZE);
        r->sector_count = len * (s->qdev.blocksize / BDRV_SECTOR_SIZE);
        break;
    default:
        abort();
    illegal_request:
        scsi_check_condition(r, SENSE_CODE(INVALID_FIELD));
        return 0;
    illegal_lba:
        scsi_check_condition(r, SENSE_CODE(LBA_OUT_OF_RANGE));
        return 0;
    }
    r->need_fua_emulation = sdc->need_fua_emulation(&r->req.cmd);
    if (r->sector_count == 0) {
        scsi_req_complete(&r->req, GOOD);
    }
    assert(r->iov.iov_len == 0);
    if (r->req.cmd.mode == SCSI_XFER_TO_DEV) {
        return -r->sector_count * BDRV_SECTOR_SIZE;
    } else {
        return r->sector_count * BDRV_SECTOR_SIZE;
    }
}