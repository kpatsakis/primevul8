static SCSIRequest *scsi_block_new_request(SCSIDevice *d, uint32_t tag,
                                           uint32_t lun, uint8_t *buf,
                                           void *hba_private)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, d);

    if (scsi_block_is_passthrough(s, buf)) {
        return scsi_req_alloc(&scsi_generic_req_ops, &s->qdev, tag, lun,
                              hba_private);
    } else {
        return scsi_req_alloc(&scsi_block_dma_reqops, &s->qdev, tag, lun,
                              hba_private);
    }
}