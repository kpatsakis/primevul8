static void cd_data_to_raw(uint8_t *buf, int lba)
{
    /* sync bytes */
    buf[0] = 0x00;
    memset(buf + 1, 0xff, 10);
    buf[11] = 0x00;
    buf += 12;
    /* MSF */
    lba_to_msf(buf, lba);
    buf[3] = 0x01; /* mode 1 data */
    buf += 4;
    /* data */
    buf += 2048;
    /* XXX: ECC not computed */
    memset(buf, 0, 288);
}