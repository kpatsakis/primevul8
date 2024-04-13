static void process_meta_command(conn *c, token_t *tokens, const size_t ntokens) {
    assert(c != NULL);

    if (tokens[KEY_TOKEN].length > KEY_MAX_LENGTH) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }

    char *key = tokens[KEY_TOKEN].value;
    size_t nkey = tokens[KEY_TOKEN].length;

    bool overflow; // not used here.
    item *it = limited_get(key, nkey, c, 0, false, DONT_UPDATE, &overflow);
    if (it) {
        mc_resp *resp = c->resp;
        size_t total = 0;
        size_t ret;
        // similar to out_string().
        memcpy(resp->wbuf, "ME ", 3);
        total += 3;
        memcpy(resp->wbuf + total, ITEM_key(it), it->nkey);
        total += it->nkey;
        resp->wbuf[total] = ' ';
        total++;

        ret = snprintf(resp->wbuf + total, WRITE_BUFFER_SIZE - (it->nkey + 12),
                "exp=%d la=%llu cas=%llu fetch=%s cls=%u size=%lu\r\n",
                (it->exptime == 0) ? -1 : (current_time - it->exptime),
                (unsigned long long)(current_time - it->time),
                (unsigned long long)ITEM_get_cas(it),
                (it->it_flags & ITEM_FETCHED) ? "yes" : "no",
                ITEM_clsid(it),
                (unsigned long) ITEM_ntotal(it));

        item_remove(it);
        resp->wbytes = total + ret;
        resp_add_iov(resp, resp->wbuf, resp->wbytes);
        conn_set_state(c, conn_new_cmd);
    } else {
        out_string(c, "EN");
    }
    pthread_mutex_lock(&c->thread->stats.mutex);
    c->thread->stats.meta_cmds++;
    pthread_mutex_unlock(&c->thread->stats.mutex);
}