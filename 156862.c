static void scsi_check_condition(SCSIDiskReq *r, SCSISense sense)
{
    trace_scsi_disk_check_condition(r->req.tag, sense.key, sense.asc,
                                    sense.ascq);
    scsi_req_build_sense(&r->req, sense);
    scsi_req_complete(&r->req, CHECK_CONDITION);
}