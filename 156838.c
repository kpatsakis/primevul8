static void scsi_disk_new_request_dump(uint32_t lun, uint32_t tag, uint8_t *buf)
{
    int i;
    int len = scsi_cdb_length(buf);
    char *line_buffer, *p;

    assert(len > 0 && len <= 16);
    line_buffer = g_malloc(len * 5 + 1);

    for (i = 0, p = line_buffer; i < len; i++) {
        p += sprintf(p, " 0x%02x", buf[i]);
    }
    trace_scsi_disk_new_request(lun, tag, line_buffer);

    g_free(line_buffer);
}