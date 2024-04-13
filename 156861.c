static void scsi_disk_emulate_read_data(SCSIRequest *req)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
    int buflen = r->iov.iov_len;

    if (buflen) {
        trace_scsi_disk_emulate_read_data(buflen);
        r->iov.iov_len = 0;
        r->started = true;
        scsi_req_data(&r->req, buflen);
        return;
    }

    /* This also clears the sense buffer for REQUEST SENSE.  */
    scsi_req_complete(&r->req, GOOD);
}