static BlockAIOCB *scsi_block_dma_readv(int64_t offset,
                                        QEMUIOVector *iov,
                                        BlockCompletionFunc *cb, void *cb_opaque,
                                        void *opaque)
{
    SCSIBlockReq *r = opaque;
    return scsi_block_do_sgio(r, offset, iov,
                              SG_DXFER_FROM_DEV, cb, cb_opaque);
}