void ide_atapi_io_error(IDEState *s, int ret)
{
    /* XXX: handle more errors */
    if (ret == -ENOMEDIUM) {
        ide_atapi_cmd_error(s, NOT_READY,
                            ASC_MEDIUM_NOT_PRESENT);
    } else {
        ide_atapi_cmd_error(s, ILLEGAL_REQUEST,
                            ASC_LOGICAL_BLOCK_OOR);
    }
}