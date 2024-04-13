static BlockAIOCB *scsi_block_dma_writev(int64_t offset,
                                         QEMUIOVector *iov,
                                         BlockCompletionFunc *cb, void *cb_opaque,
                                         void *opaque)
{
    SCSIBlockReq *r = opaque;
    return scsi_block_do_sgio(r, offset, iov,
                              SG_DXFER_TO_DEV, cb, cb_opaque);
}