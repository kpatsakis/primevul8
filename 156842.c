static void scsi_write_complete_noio(SCSIDiskReq *r, int ret)
{
    uint32_t n;

    assert (r->req.aiocb == NULL);
    if (scsi_disk_req_check_error(r, ret, false)) {
        goto done;
    }

    n = r->qiov.size / BDRV_SECTOR_SIZE;
    r->sector += n;
    r->sector_count -= n;
    if (r->sector_count == 0) {
        scsi_write_do_fua(r);
        return;
    } else {
        scsi_init_iovec(r, SCSI_DMA_BUF_SIZE);
        trace_scsi_disk_write_complete_noio(r->req.tag, r->qiov.size);
        scsi_req_data(&r->req, r->qiov.size);
    }

done:
    scsi_req_unref(&r->req);
}