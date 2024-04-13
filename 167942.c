static void cmd_test_unit_ready(IDEState *s, uint8_t *buf)
{
    /* Not Ready Conditions are already handled in ide_atapi_cmd(), so if we
     * come here, we know that it's ready. */
    ide_atapi_cmd_ok(s);
}