static SCSIRequest *scsi_new_request(SCSIDevice *d, uint32_t tag, uint32_t lun,
                                     uint8_t *buf, void *hba_private)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, d);
    SCSIRequest *req;
    const SCSIReqOps *ops;
    uint8_t command;

    command = buf[0];
    ops = scsi_disk_reqops_dispatch[command];
    if (!ops) {
        ops = &scsi_disk_emulate_reqops;
    }
    req = scsi_req_alloc(ops, &s->qdev, tag, lun, hba_private);

    if (trace_event_get_state_backends(TRACE_SCSI_DISK_NEW_REQUEST)) {
        scsi_disk_new_request_dump(lun, tag, buf);
    }

    return req;
}