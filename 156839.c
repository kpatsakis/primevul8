static void scsi_block_sgio_complete(void *opaque, int ret)
{
    SCSIBlockReq *req = (SCSIBlockReq *)opaque;
    SCSIDiskReq *r = &req->req;
    SCSIDevice *s = r->req.dev;
    sg_io_hdr_t *io_hdr = &req->io_header;

    if (ret == 0) {
        if (io_hdr->host_status != SCSI_HOST_OK) {
            scsi_req_complete_failed(&r->req, io_hdr->host_status);
            scsi_req_unref(&r->req);
            return;
        }

        if (io_hdr->driver_status & SG_ERR_DRIVER_TIMEOUT) {
            ret = BUSY;
        } else {
            ret = io_hdr->status;
        }

        if (ret > 0) {
            aio_context_acquire(blk_get_aio_context(s->conf.blk));
            if (scsi_handle_rw_error(r, ret, true)) {
                aio_context_release(blk_get_aio_context(s->conf.blk));
                scsi_req_unref(&r->req);
                return;
            }
            aio_context_release(blk_get_aio_context(s->conf.blk));

            /* Ignore error.  */
            ret = 0;
        }
    }

    req->cb(req->cb_opaque, ret);
}