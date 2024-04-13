static void rbuf_release(conn *c) {
    if (c->rbuf != NULL && c->rbytes == 0 && !IS_UDP(c->transport)) {
        if (c->rbuf_malloced) {
            free(c->rbuf);
            c->rbuf_malloced = false;
        } else {
            do_cache_free(c->thread->rbuf_cache, c->rbuf);
        }
        c->rsize = 0;
        c->rbuf = NULL;
        c->rcurr = NULL;
    }
}