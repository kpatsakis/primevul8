BlockAIOCB *scsi_dma_readv(int64_t offset, QEMUIOVector *iov,
                           BlockCompletionFunc *cb, void *cb_opaque,
                           void *opaque)
{
    SCSIDiskReq *r = opaque;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);
    return blk_aio_preadv(s->qdev.conf.blk, offset, iov, 0, cb, cb_opaque);
}