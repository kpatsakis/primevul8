static void scsi_dma_complete(void *opaque, int ret)
{
    SCSIDiskReq *r = (SCSIDiskReq *)opaque;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    assert(r->req.aiocb != NULL);
    r->req.aiocb = NULL;

    aio_context_acquire(blk_get_aio_context(s->qdev.conf.blk));
    if (ret < 0) {
        block_acct_failed(blk_get_stats(s->qdev.conf.blk), &r->acct);
    } else {
        block_acct_done(blk_get_stats(s->qdev.conf.blk), &r->acct);
    }
    scsi_dma_complete_noio(r, ret);
    aio_context_release(blk_get_aio_context(s->qdev.conf.blk));
}