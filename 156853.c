static void scsi_dma_complete_noio(SCSIDiskReq *r, int ret)
{
    assert(r->req.aiocb == NULL);
    if (scsi_disk_req_check_error(r, ret, false)) {
        goto done;
    }

    r->sector += r->sector_count;
    r->sector_count = 0;
    if (r->req.cmd.mode == SCSI_XFER_TO_DEV) {
        scsi_write_do_fua(r);
        return;
    } else {
        scsi_req_complete(&r->req, GOOD);
    }

done:
    scsi_req_unref(&r->req);
}