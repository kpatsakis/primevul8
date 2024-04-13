static void scsi_unmap_complete(void *opaque, int ret)
{
    UnmapCBData *data = opaque;
    SCSIDiskReq *r = data->r;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    assert(r->req.aiocb != NULL);
    r->req.aiocb = NULL;

    aio_context_acquire(blk_get_aio_context(s->qdev.conf.blk));
    if (scsi_disk_req_check_error(r, ret, true)) {
        scsi_req_unref(&r->req);
        g_free(data);
    } else {
        block_acct_done(blk_get_stats(s->qdev.conf.blk), &r->acct);
        scsi_unmap_complete_noio(data, ret);
    }
    aio_context_release(blk_get_aio_context(s->qdev.conf.blk));
}