static int32_t scsi_disk_emulate_command(SCSIRequest *req, uint8_t *buf)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, req->dev);
    uint64_t nb_sectors;
    uint8_t *outbuf;
    int buflen;

    switch (req->cmd.buf[0]) {
    case INQUIRY:
    case MODE_SENSE:
    case MODE_SENSE_10:
    case RESERVE:
    case RESERVE_10:
    case RELEASE:
    case RELEASE_10:
    case START_STOP:
    case ALLOW_MEDIUM_REMOVAL:
    case GET_CONFIGURATION:
    case GET_EVENT_STATUS_NOTIFICATION:
    case MECHANISM_STATUS:
    case REQUEST_SENSE:
        break;

    default:
        if (!blk_is_available(s->qdev.conf.blk)) {
            scsi_check_condition(r, SENSE_CODE(NO_MEDIUM));
            return 0;
        }
        break;
    }

    /*
     * FIXME: we shouldn't return anything bigger than 4k, but the code
     * requires the buffer to be as big as req->cmd.xfer in several
     * places.  So, do not allow CDBs with a very large ALLOCATION
     * LENGTH.  The real fix would be to modify scsi_read_data and
     * dma_buf_read, so that they return data beyond the buflen
     * as all zeros.
     */
    if (req->cmd.xfer > 65536) {
        goto illegal_request;
    }
    r->buflen = MAX(4096, req->cmd.xfer);

    if (!r->iov.iov_base) {
        r->iov.iov_base = blk_blockalign(s->qdev.conf.blk, r->buflen);
    }

    outbuf = r->iov.iov_base;
    memset(outbuf, 0, r->buflen);
    switch (req->cmd.buf[0]) {
    case TEST_UNIT_READY:
        assert(blk_is_available(s->qdev.conf.blk));
        break;
    case INQUIRY:
        buflen = scsi_disk_emulate_inquiry(req, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case MODE_SENSE:
    case MODE_SENSE_10:
        buflen = scsi_disk_emulate_mode_sense(r, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case READ_TOC:
        buflen = scsi_disk_emulate_read_toc(req, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case RESERVE:
        if (req->cmd.buf[1] & 1) {
            goto illegal_request;
        }
        break;
    case RESERVE_10:
        if (req->cmd.buf[1] & 3) {
            goto illegal_request;
        }
        break;
    case RELEASE:
        if (req->cmd.buf[1] & 1) {
            goto illegal_request;
        }
        break;
    case RELEASE_10:
        if (req->cmd.buf[1] & 3) {
            goto illegal_request;
        }
        break;
    case START_STOP:
        if (scsi_disk_emulate_start_stop(r) < 0) {
            return 0;
        }
        break;
    case ALLOW_MEDIUM_REMOVAL:
        s->tray_locked = req->cmd.buf[4] & 1;
        blk_lock_medium(s->qdev.conf.blk, req->cmd.buf[4] & 1);
        break;
    case READ_CAPACITY_10:
        /* The normal LEN field for this command is zero.  */
        memset(outbuf, 0, 8);
        blk_get_geometry(s->qdev.conf.blk, &nb_sectors);
        if (!nb_sectors) {
            scsi_check_condition(r, SENSE_CODE(LUN_NOT_READY));
            return 0;
        }
        if ((req->cmd.buf[8] & 1) == 0 && req->cmd.lba) {
            goto illegal_request;
        }
        nb_sectors /= s->qdev.blocksize / BDRV_SECTOR_SIZE;
        /* Returned value is the address of the last sector.  */
        nb_sectors--;
        /* Remember the new size for read/write sanity checking. */
        s->qdev.max_lba = nb_sectors;
        /* Clip to 2TB, instead of returning capacity modulo 2TB. */
        if (nb_sectors > UINT32_MAX) {
            nb_sectors = UINT32_MAX;
        }
        outbuf[0] = (nb_sectors >> 24) & 0xff;
        outbuf[1] = (nb_sectors >> 16) & 0xff;
        outbuf[2] = (nb_sectors >> 8) & 0xff;
        outbuf[3] = nb_sectors & 0xff;
        outbuf[4] = 0;
        outbuf[5] = 0;
        outbuf[6] = s->qdev.blocksize >> 8;
        outbuf[7] = 0;
        break;
    case REQUEST_SENSE:
        /* Just return "NO SENSE".  */
        buflen = scsi_convert_sense(NULL, 0, outbuf, r->buflen,
                                    (req->cmd.buf[1] & 1) == 0);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case MECHANISM_STATUS:
        buflen = scsi_emulate_mechanism_status(s, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case GET_CONFIGURATION:
        buflen = scsi_get_configuration(s, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case GET_EVENT_STATUS_NOTIFICATION:
        buflen = scsi_get_event_status_notification(s, r, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case READ_DISC_INFORMATION:
        buflen = scsi_read_disc_information(s, r, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case READ_DVD_STRUCTURE:
        buflen = scsi_read_dvd_structure(s, r, outbuf);
        if (buflen < 0) {
            goto illegal_request;
        }
        break;
    case SERVICE_ACTION_IN_16:
        /* Service Action In subcommands. */
        if ((req->cmd.buf[1] & 31) == SAI_READ_CAPACITY_16) {
            trace_scsi_disk_emulate_command_SAI_16();
            memset(outbuf, 0, req->cmd.xfer);
            blk_get_geometry(s->qdev.conf.blk, &nb_sectors);
            if (!nb_sectors) {
                scsi_check_condition(r, SENSE_CODE(LUN_NOT_READY));
                return 0;
            }
            if ((req->cmd.buf[14] & 1) == 0 && req->cmd.lba) {
                goto illegal_request;
            }
            nb_sectors /= s->qdev.blocksize / BDRV_SECTOR_SIZE;
            /* Returned value is the address of the last sector.  */
            nb_sectors--;
            /* Remember the new size for read/write sanity checking. */
            s->qdev.max_lba = nb_sectors;
            outbuf[0] = (nb_sectors >> 56) & 0xff;
            outbuf[1] = (nb_sectors >> 48) & 0xff;
            outbuf[2] = (nb_sectors >> 40) & 0xff;
            outbuf[3] = (nb_sectors >> 32) & 0xff;
            outbuf[4] = (nb_sectors >> 24) & 0xff;
            outbuf[5] = (nb_sectors >> 16) & 0xff;
            outbuf[6] = (nb_sectors >> 8) & 0xff;
            outbuf[7] = nb_sectors & 0xff;
            outbuf[8] = 0;
            outbuf[9] = 0;
            outbuf[10] = s->qdev.blocksize >> 8;
            outbuf[11] = 0;
            outbuf[12] = 0;
            outbuf[13] = get_physical_block_exp(&s->qdev.conf);

            /* set TPE bit if the format supports discard */
            if (s->qdev.conf.discard_granularity) {
                outbuf[14] = 0x80;
            }

            /* Protection, exponent and lowest lba field left blank. */
            break;
        }
        trace_scsi_disk_emulate_command_SAI_unsupported();
        goto illegal_request;
    case SYNCHRONIZE_CACHE:
        /* The request is used as the AIO opaque value, so add a ref.  */
        scsi_req_ref(&r->req);
        block_acct_start(blk_get_stats(s->qdev.conf.blk), &r->acct, 0,
                         BLOCK_ACCT_FLUSH);
        r->req.aiocb = blk_aio_flush(s->qdev.conf.blk, scsi_aio_complete, r);
        return 0;
    case SEEK_10:
        trace_scsi_disk_emulate_command_SEEK_10(r->req.cmd.lba);
        if (r->req.cmd.lba > s->qdev.max_lba) {
            goto illegal_lba;
        }
        break;
    case MODE_SELECT:
        trace_scsi_disk_emulate_command_MODE_SELECT(r->req.cmd.xfer);
        break;
    case MODE_SELECT_10:
        trace_scsi_disk_emulate_command_MODE_SELECT_10(r->req.cmd.xfer);
        break;
    case UNMAP:
        trace_scsi_disk_emulate_command_UNMAP(r->req.cmd.xfer);
        break;
    case VERIFY_10:
    case VERIFY_12:
    case VERIFY_16:
        trace_scsi_disk_emulate_command_VERIFY((req->cmd.buf[1] >> 1) & 3);
        if (req->cmd.buf[1] & 6) {
            goto illegal_request;
        }
        break;
    case WRITE_SAME_10:
    case WRITE_SAME_16:
        trace_scsi_disk_emulate_command_WRITE_SAME(
                req->cmd.buf[0] == WRITE_SAME_10 ? 10 : 16, r->req.cmd.xfer);
        break;
    default:
        trace_scsi_disk_emulate_command_UNKNOWN(buf[0],
                                                scsi_command_name(buf[0]));
        scsi_check_condition(r, SENSE_CODE(INVALID_OPCODE));
        return 0;
    }
    assert(!r->req.aiocb);
    r->iov.iov_len = MIN(r->buflen, req->cmd.xfer);
    if (r->iov.iov_len == 0) {
        scsi_req_complete(&r->req, GOOD);
    }
    if (r->req.cmd.mode == SCSI_XFER_TO_DEV) {
        assert(r->iov.iov_len == req->cmd.xfer);
        return -r->iov.iov_len;
    } else {
        return r->iov.iov_len;
    }

illegal_request:
    if (r->req.status == -1) {
        scsi_check_condition(r, SENSE_CODE(INVALID_FIELD));
    }
    return 0;

illegal_lba:
    scsi_check_condition(r, SENSE_CODE(LBA_OUT_OF_RANGE));
    return 0;
}