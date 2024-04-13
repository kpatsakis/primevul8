static void cmd_get_event_status_notification(IDEState *s,
                                              uint8_t *buf)
{
    const uint8_t *packet = buf;

    struct {
        uint8_t opcode;
        uint8_t polled;        /* lsb bit is polled; others are reserved */
        uint8_t reserved2[2];
        uint8_t class;
        uint8_t reserved3[2];
        uint16_t len;
        uint8_t control;
    } QEMU_PACKED *gesn_cdb;

    struct {
        uint16_t len;
        uint8_t notification_class;
        uint8_t supported_events;
    } QEMU_PACKED *gesn_event_header;
    unsigned int max_len, used_len;

    gesn_cdb = (void *)packet;
    gesn_event_header = (void *)buf;

    max_len = be16_to_cpu(gesn_cdb->len);

    /* It is fine by the MMC spec to not support async mode operations */
    if (!(gesn_cdb->polled & 0x01)) { /* asynchronous mode */
        /* Only polling is supported, asynchronous mode is not. */
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                            ASC_INV_FIELD_IN_CMD_PACKET);
        return;
    }

    /* polling mode operation */

    /*
     * These are the supported events.
     *
     * We currently only support requests of the 'media' type.
     * Notification class requests and supported event classes are bitmasks,
     * but they are build from the same values as the "notification class"
     * field.
     */
    gesn_event_header->supported_events = 1 << GESN_MEDIA;

    /*
     * We use |= below to set the class field; other bits in this byte
     * are reserved now but this is useful to do if we have to use the
     * reserved fields later.
     */
    gesn_event_header->notification_class = 0;

    /*
     * Responses to requests are to be based on request priority.  The
     * notification_class_request_type enum above specifies the
     * priority: upper elements are higher prio than lower ones.
     */
    if (gesn_cdb->class & (1 << GESN_MEDIA)) {
        gesn_event_header->notification_class |= GESN_MEDIA;
        used_len = event_status_media(s, buf);
    } else {
        gesn_event_header->notification_class = 0x80; /* No event available */
        used_len = sizeof(*gesn_event_header);
    }
    gesn_event_header->len = cpu_to_be16(used_len
                                         - sizeof(*gesn_event_header));
    ide_atapi_cmd_reply(s, used_len, max_len);
}