static void cmd_prevent_allow_medium_removal(IDEState *s, uint8_t* buf)
{
    s->tray_locked = buf[4] & 1;
    blk_lock_medium(s->blk, buf[4] & 1);
    ide_atapi_cmd_ok(s);
}