static void scsi_aio_complete(void *opaque, int ret)
{
    SCSIDiskReq *r = (SCSIDiskReq *)opaque;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    assert(r->req.aiocb != NULL);
    r->req.aiocb = NULL;
    aio_context_acquire(blk_get_aio_context(s->qdev.conf.blk));
    if (scsi_disk_req_check_error(r, ret, true)) {
        goto done;
    }

    block_acct_done(blk_get_stats(s->qdev.conf.blk), &r->acct);
    scsi_req_complete(&r->req, GOOD);

done:
    aio_context_release(blk_get_aio_context(s->qdev.conf.blk));
    scsi_req_unref(&r->req);
}