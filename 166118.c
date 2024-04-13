sds sdstrynewlen(const void *init, size_t initlen) {
    return _sdsnewlen(init, initlen, 1);
}