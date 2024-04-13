static void scsi_read_complete_noio(SCSIDiskReq *r, int ret)
{
    uint32_t n;

    assert(r->req.aiocb == NULL);
    if (scsi_disk_req_check_error(r, ret, false)) {
        goto done;
    }

    n = r->qiov.size / BDRV_SECTOR_SIZE;
    r->sector += n;
    r->sector_count -= n;
    scsi_req_data(&r->req, r->qiov.size);

done:
    scsi_req_unref(&r->req);
}