static int scsi_get_event_status_notification(SCSIDiskState *s, SCSIDiskReq *r,
                                              uint8_t *outbuf)
{
    int size;
    uint8_t *buf = r->req.cmd.buf;
    uint8_t notification_class_request = buf[4];
    if (s->qdev.type != TYPE_ROM) {
        return -1;
    }
    if ((buf[1] & 1) == 0) {
        /* asynchronous */
        return -1;
    }

    size = 4;
    outbuf[0] = outbuf[1] = 0;
    outbuf[3] = 1 << GESN_MEDIA; /* supported events */
    if (notification_class_request & (1 << GESN_MEDIA)) {
        outbuf[2] = GESN_MEDIA;
        size += scsi_event_status_media(s, &outbuf[size]);
    } else {
        outbuf[2] = 0x80;
    }
    stw_be_p(outbuf, size - 4);
    return size;
}