static bool scsi_disk_req_check_error(SCSIDiskReq *r, int ret, bool acct_failed)
{
    if (r->req.io_canceled) {
        scsi_req_cancel_complete(&r->req);
        return true;
    }

    if (ret < 0) {
        return scsi_handle_rw_error(r, ret, acct_failed);
    }

    return false;
}