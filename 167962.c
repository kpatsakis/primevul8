static void cmd_request_sense(IDEState *s, uint8_t *buf)
{
    int max_len = buf[4];

    memset(buf, 0, 18);
    buf[0] = 0x70 | (1 << 7);
    buf[2] = s->sense_key;
    buf[7] = 10;
    buf[12] = s->asc;

    if (s->sense_key == UNIT_ATTENTION) {
        s->sense_key = NO_SENSE;
    }

    ide_atapi_cmd_reply(s, 18, max_len);
}