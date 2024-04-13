static bool rbuf_switch_to_malloc(conn *c) {
    // Might as well start with x2 and work from there.
    size_t size = c->rsize * 2;
    char *tmp = malloc(size);
    if (!tmp)
        return false;

    do_cache_free(c->thread->rbuf_cache, c->rbuf);
    memcpy(tmp, c->rcurr, c->rbytes);

    c->rcurr = c->rbuf = tmp;
    c->rsize = size;
    c->rbuf_malloced = true;
    return true;
}