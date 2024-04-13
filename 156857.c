static void scsi_write_do_fua(SCSIDiskReq *r)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    assert(r->req.aiocb == NULL);
    assert(!r->req.io_canceled);

    if (r->need_fua_emulation) {
        block_acct_start(blk_get_stats(s->qdev.conf.blk), &r->acct, 0,
                         BLOCK_ACCT_FLUSH);
        r->req.aiocb = blk_aio_flush(s->qdev.conf.blk, scsi_aio_complete, r);
        return;
    }

    scsi_req_complete(&r->req, GOOD);
    scsi_req_unref(&r->req);
}