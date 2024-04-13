static void scsi_disk_emulate_write_data(SCSIRequest *req)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);

    if (r->iov.iov_len) {
        int buflen = r->iov.iov_len;
        trace_scsi_disk_emulate_write_data(buflen);
        r->iov.iov_len = 0;
        scsi_req_data(&r->req, buflen);
        return;
    }

    switch (req->cmd.buf[0]) {
    case MODE_SELECT:
    case MODE_SELECT_10:
        /* This also clears the sense buffer for REQUEST SENSE.  */
        scsi_disk_emulate_mode_select(r, r->iov.iov_base);
        break;

    case UNMAP:
        scsi_disk_emulate_unmap(r, r->iov.iov_base);
        break;

    case VERIFY_10:
    case VERIFY_12:
    case VERIFY_16:
        if (r->req.status == -1) {
            scsi_check_condition(r, SENSE_CODE(INVALID_FIELD));
        }
        break;

    case WRITE_SAME_10:
    case WRITE_SAME_16:
        scsi_disk_emulate_write_same(r, r->iov.iov_base);
        break;

    default:
        abort();
    }
}