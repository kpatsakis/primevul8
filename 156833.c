static int mode_select_pages(SCSIDiskReq *r, uint8_t *p, int len, bool change)
{
    SCSIDiskState *s = DO_UPCAST(SCSIDiskState, qdev, r->req.dev);

    while (len > 0) {
        int page, subpage, page_len;

        /* Parse both possible formats for the mode page headers.  */
        page = p[0] & 0x3f;
        if (p[0] & 0x40) {
            if (len < 4) {
                goto invalid_param_len;
            }
            subpage = p[1];
            page_len = lduw_be_p(&p[2]);
            p += 4;
            len -= 4;
        } else {
            if (len < 2) {
                goto invalid_param_len;
            }
            subpage = 0;
            page_len = p[1];
            p += 2;
            len -= 2;
        }

        if (subpage) {
            goto invalid_param;
        }
        if (page_len > len) {
            goto invalid_param_len;
        }

        if (!change) {
            if (scsi_disk_check_mode_select(s, page, p, page_len) < 0) {
                goto invalid_param;
            }
        } else {
            scsi_disk_apply_mode_select(s, page, p);
        }

        p += page_len;
        len -= page_len;
    }
    return 0;

invalid_param:
    scsi_check_condition(r, SENSE_CODE(INVALID_PARAM));
    return -1;

invalid_param_len:
    scsi_check_condition(r, SENSE_CODE(INVALID_PARAM_LEN));
    return -1;
}