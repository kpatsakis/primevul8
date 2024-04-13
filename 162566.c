is_known_multipart_header(const char *header_str, guint len)
{
    guint i;

    for (i = 1; i < array_length(multipart_headers); i++) {
        if (len == strlen(multipart_headers[i].name) &&
            g_ascii_strncasecmp(header_str, multipart_headers[i].name, len) == 0)
            return i;
        if (multipart_headers[i].compact_name != NULL &&
            len == strlen(multipart_headers[i].compact_name) &&
            g_ascii_strncasecmp(header_str, multipart_headers[i].compact_name, len) == 0)
            return i;
    }

    return -1;
}