kex_method_list(unsigned char *buf, size_t list_strlen,
                LIBSSH2_COMMON_METHOD ** method)
{
    _libssh2_htonu32(buf, list_strlen);
    buf += 4;

    if (!method || !*method) {
        return 4;
    }

    while (*method && (*method)->name) {
        int mlen = strlen((*method)->name);
        memcpy(buf, (*method)->name, mlen);
        buf += mlen;
        *(buf++) = ',';
        method++;
    }

    return list_strlen + 4;
}