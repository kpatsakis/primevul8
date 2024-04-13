static void scsi_block_update_sense(SCSIRequest *req)
{
    SCSIDiskReq *r = DO_UPCAST(SCSIDiskReq, req, req);
    SCSIBlockReq *br = DO_UPCAST(SCSIBlockReq, req, r);
    r->req.sense_len = MIN(br->io_header.sb_len_wr, sizeof(r->req.sense));
}