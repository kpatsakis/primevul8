static void lba_to_msf(uint8_t *buf, int lba)
{
    lba += 150;
    buf[0] = (lba / 75) / 60;
    buf[1] = (lba / 75) % 60;
    buf[2] = lba % 75;
}