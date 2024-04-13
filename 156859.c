static int scsi_disk_emulate_start_stop(SCSIDiskReq *r)
{
    SCSIRequest *req = &r->req;
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, req->dev);
    bool start = req->cmd.buf[4] & 1;
    bool loej = req->cmd.buf[4] & 2; /* load on start, eject on !start */
    int pwrcnd = req->cmd.buf[4] & 0xf0;

    if (pwrcnd) {
        /* eject/load only happens for power condition == 0 */
        return 0;
    }

    if ((s->features & (1 << SCSI_DISK_F_REMOVABLE)) && loej) {
        if (!start && !s->tray_open && s->tray_locked) {
            scsi_check_condition(r,
                                 blk_is_inserted(s->qdev.conf.blk)
                                 ? SENSE_CODE(ILLEGAL_REQ_REMOVAL_PREVENTED)
                                 : SENSE_CODE(NOT_READY_REMOVAL_PREVENTED));
            return -1;
        }

        if (s->tray_open != !start) {
            blk_eject(s->qdev.conf.blk, !start);
            s->tray_open = !start;
        }
    }
    return 0;
}