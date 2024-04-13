static int seed_from_urandom(uint32_t *seed) {
    /* Use unbuffered I/O if we have open(), close() and read(). Otherwise
       fall back to fopen() */

    char data[sizeof(uint32_t)];
    int ok;

#if defined(HAVE_OPEN) && defined(HAVE_CLOSE) && defined(HAVE_READ)
    int urandom;
    urandom = open("/dev/urandom", O_RDONLY);
    if (urandom == -1)
        return 1;

    ok = read(urandom, data, sizeof(uint32_t)) == sizeof(uint32_t);
    close(urandom);
#else
    FILE *urandom;

    urandom = fopen("/dev/urandom", "rb");
    if (!urandom)
        return 1;

    ok = fread(data, 1, sizeof(uint32_t), urandom) == sizeof(uint32_t);
    fclose(urandom);
#endif

    if (!ok)
        return 1;

    *seed = buf_to_uint32(data);
    return 0;
}