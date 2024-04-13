static void scsi_read_data(SCSIRequest *req)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);
    bool first;

    trace_scsi_disk_read_data_count(r->sector_count);
    if (r->sector_count == 0) {
        /* This also clears the sense buffer for REQUEST SENSE.  */
        scsi_req_complete(&r->req, GOOD);
        return;
    }

    /* No data transfer may already be in progress */
    assert(r->req.aiocb == NULL);

    /* The request is used as the AIO opaque value, so add a ref.  */
    scsi_req_ref(&r->req);
    if (r->req.cmd.mode == SCSI_XFER_TO_DEV) {
        trace_scsi_disk_read_data_invalid();
        scsi_read_complete_noio(r, -EINVAL);
        return;
    }

    if (!blk_is_available(req->dev->conf.blk)) {
        scsi_read_complete_noio(r, -ENOMEDIUM);
        return;
    }

    first = !r->started;
    r->started = true;
    if (first && r->need_fua_emulation) {
        block_acct_start(blk_get_stats(s->qdev.conf.blk), &r->acct, 0,
                         BLOCK_ACCT_FLUSH);
        r->req.aiocb = blk_aio_flush(s->qdev.conf.blk, scsi_do_read_cb, r);
    } else {
        scsi_do_read(r, 0);
    }
}