nm_utils_dnsmasq_status_to_string(int status, char *dest, gsize size)
{
    const char *msg;

    nm_utils_to_string_buffer_init(&dest, &size);

    if (status == 0)
        msg = "Success";
    else if (status == 1)
        msg = "Configuration problem";
    else if (status == 2)
        msg = "Network access problem (address in use, permissions)";
    else if (status == 3)
        msg = "Filesystem problem (missing file/directory, permissions)";
    else if (status == 4)
        msg = "Memory allocation failure";
    else if (status == 5)
        msg = "Other problem";
    else if (status >= 11) {
        g_snprintf(dest, size, "Lease script failed with error %d", status - 10);
        return dest;
    } else
        msg = "Unknown problem";

    g_snprintf(dest, size, "%s (%d)", msg, status);
    return dest;
}