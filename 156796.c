static void scsi_init_iovec(SCSIDiskReq *r, size_t size)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    if (!r->iov.iov_base) {
        r->buflen = size;
        r->iov.iov_base = blk_blockalign(s->qdev.conf.blk, r->buflen);
    }
    r->iov.iov_len = MIN(r->sector_count * BDRV_SECTOR_SIZE, r->buflen);
    qemu_iovec_init_external(&r->qiov, &r->iov, 1);
}