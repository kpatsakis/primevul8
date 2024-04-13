static bool rbuf_alloc(conn *c) {
    if (c->rbuf == NULL) {
        c->rbuf = do_cache_alloc(c->thread->rbuf_cache);
        if (!c->rbuf) {
            THR_STATS_LOCK(c);
            c->thread->stats.read_buf_oom++;
            THR_STATS_UNLOCK(c);
            return false;
        }
        c->rsize = READ_BUFFER_SIZE;
        c->rcurr = c->rbuf;
    }
    return true;
}