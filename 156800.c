static int scsi_read_disc_information(SCSIDiskState *s, SCSIDiskReq *r,
                                      uint8_t *outbuf)
{
    uint8_t type = r->req.cmd.buf[1] & 7;

    if (s->qdev.type != TYPE_ROM) {
        return -1;
    }

    /* Types 1/2 are only defined for Blu-Ray.  */
    if (type != 0) {
        scsi_check_condition(r, SENSE_CODE(INVALID_FIELD));
        return -1;
    }

    memset(outbuf, 0, 34);
    outbuf[1] = 32;
    outbuf[2] = 0xe; /* last session complete, disc finalized */
    outbuf[3] = 1;   /* first track on disc */
    outbuf[4] = 1;   /* # of sessions */
    outbuf[5] = 1;   /* first track of last session */
    outbuf[6] = 1;   /* last track of last session */
    outbuf[7] = 0x20; /* unrestricted use */
    outbuf[8] = 0x00; /* CD-ROM or DVD-ROM */
    /* 9-10-11: most significant byte corresponding bytes 4-5-6 */
    /* 12-23: not meaningful for CD-ROM or DVD-ROM */
    /* 24-31: disc bar code */
    /* 32: disc application code */
    /* 33: number of OPC tables */

    return 34;
}