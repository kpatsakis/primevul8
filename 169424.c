kex_method_strlen(LIBSSH2_COMMON_METHOD ** method)
{
    size_t len = 0;

    if (!method || !*method) {
        return 0;
    }

    while (*method && (*method)->name) {
        len += strlen((*method)->name) + 1;
        method++;
    }

    return len - 1;
}