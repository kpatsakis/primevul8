static BlockAIOCB *scsi_block_do_sgio(SCSIBlockReq *req,
                                      int64_t offset, QEMUIOVector *iov,
                                      int direction,
                                      BlockCompletionFunc *cb, void *opaque)
{
    sg_io_hdr_t *io_header = &req->io_header;
    SCSIDiskReq *r = &req->req;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);
    int nb_logical_blocks;
    uint64_t lba;
    BlockAIOCB *aiocb;

    /* This is not supported yet.  It can only happen if the guest does
     * reads and writes that are not aligned to one logical sectors
     * _and_ cover multiple MemoryRegions.
     */
    assert(offset % s->qdev.blocksize == 0);
    assert(iov->size % s->qdev.blocksize == 0);

    io_header->interface_id = 'S';

    /* The data transfer comes from the QEMUIOVector.  */
    io_header->dxfer_direction = direction;
    io_header->dxfer_len = iov->size;
    io_header->dxferp = (void *)iov->iov;
    io_header->iovec_count = iov->niov;
    assert(io_header->iovec_count == iov->niov); /* no overflow! */

    /* Build a new CDB with the LBA and length patched in, in case
     * DMA helpers split the transfer in multiple segments.  Do not
     * build a CDB smaller than what the guest wanted, and only build
     * a larger one if strictly necessary.
     */
    io_header->cmdp = req->cdb;
    lba = offset / s->qdev.blocksize;
    nb_logical_blocks = io_header->dxfer_len / s->qdev.blocksize;

    if ((req->cmd >> 5) == 0 && lba <= 0x1ffff) {
        /* 6-byte CDB */
        stl_be_p(&req->cdb[0], lba | (req->cmd << 24));
        req->cdb[4] = nb_logical_blocks;
        req->cdb[5] = 0;
        io_header->cmd_len = 6;
    } else if ((req->cmd >> 5) <= 1 && lba <= 0xffffffffULL) {
        /* 10-byte CDB */
        req->cdb[0] = (req->cmd & 0x1f) | 0x20;
        req->cdb[1] = req->cdb1;
        stl_be_p(&req->cdb[2], lba);
        req->cdb[6] = req->group_number;
        stw_be_p(&req->cdb[7], nb_logical_blocks);
        req->cdb[9] = 0;
        io_header->cmd_len = 10;
    } else if ((req->cmd >> 5) != 4 && lba <= 0xffffffffULL) {
        /* 12-byte CDB */
        req->cdb[0] = (req->cmd & 0x1f) | 0xA0;
        req->cdb[1] = req->cdb1;
        stl_be_p(&req->cdb[2], lba);
        stl_be_p(&req->cdb[6], nb_logical_blocks);
        req->cdb[10] = req->group_number;
        req->cdb[11] = 0;
        io_header->cmd_len = 12;
    } else {
        /* 16-byte CDB */
        req->cdb[0] = (req->cmd & 0x1f) | 0x80;
        req->cdb[1] = req->cdb1;
        stq_be_p(&req->cdb[2], lba);
        stl_be_p(&req->cdb[10], nb_logical_blocks);
        req->cdb[14] = req->group_number;
        req->cdb[15] = 0;
        io_header->cmd_len = 16;
    }

    /* The rest is as in scsi-generic.c.  */
    io_header->mx_sb_len = sizeof(r->req.sense);
    io_header->sbp = r->req.sense;
    io_header->timeout = s->qdev.io_timeout * 1000;
    io_header->usr_ptr = r;
    io_header->flags |= SG_FLAG_DIRECT_IO;
    req->cb = cb;
    req->cb_opaque = opaque;
    trace_scsi_disk_aio_sgio_command(r->req.tag, req->cdb[0], lba,
                                     nb_logical_blocks, io_header->timeout);
    aiocb = blk_aio_ioctl(s->qdev.conf.blk, SG_IO, io_header, scsi_block_sgio_complete, req);
    assert(aiocb != NULL);
    return aiocb;
}