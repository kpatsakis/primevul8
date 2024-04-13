static void process_mdelete_command(conn *c, token_t *tokens, const size_t ntokens) {
    char *key;
    size_t nkey;
    uint64_t req_cas_id = 0;
    item *it = NULL;
    int i;
    uint32_t hv;
    struct _meta_flags of = {0}; // option bitflags.
    char *errstr = "CLIENT_ERROR bad command line format";
    mc_resp *resp = c->resp;
    // reserve 3 bytes for status code
    char *p = resp->wbuf + 3;

    assert(c != NULL);
    WANT_TOKENS_MIN(ntokens, 3);

    // TODO: most of this is identical to mget.
    if (tokens[KEY_TOKEN].length > KEY_MAX_LENGTH) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }

    key = tokens[KEY_TOKEN].value;
    nkey = tokens[KEY_TOKEN].length;

    if (ntokens > MFLAG_MAX_OPT_LENGTH) {
        out_string(c, "CLIENT_ERROR options flags too long");
        return;
    }

    // scrubs duplicated options and sets flags for how to load the item.
    if (_meta_flag_preparse(tokens, ntokens, &of, &errstr) != 0) {
        out_errstring(c, "CLIENT_ERROR invalid or duplicate flag");
        return;
    }
    c->noreply = of.no_reply;

    assert(c != NULL);
    for (i = KEY_TOKEN+1; i < ntokens-1; i++) {
        switch (tokens[i].value[0]) {
            // TODO: macro perhaps?
            case 'O':
                if (tokens[i].length > MFLAG_MAX_OPAQUE_LENGTH) {
                    errstr = "CLIENT_ERROR opaque token too long";
                    goto error;
                }
                META_SPACE(p);
                memcpy(p, tokens[i].value, tokens[i].length);
                p += tokens[i].length;
                break;
            case 'k':
                META_CHAR(p, 'k');
                memcpy(p, key, nkey);
                p += nkey;
                break;
        }
    }

    it = item_get_locked(key, nkey, c, DONT_UPDATE, &hv);
    if (it) {
        MEMCACHED_COMMAND_DELETE(c->sfd, ITEM_key(it), it->nkey);

        // allow only deleting/marking if a CAS value matches.
        if (of.has_cas && ITEM_get_cas(it) != req_cas_id) {
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.delete_misses++;
            pthread_mutex_unlock(&c->thread->stats.mutex);

            memcpy(resp->wbuf, "EX ", 3);
            goto cleanup;
        }

        // If we're to set this item as stale, we don't actually want to
        // delete it. We mark the stale bit, bump CAS, and update exptime if
        // we were supplied a new TTL.
        if (of.set_stale) {
            if (of.new_ttl) {
                it->exptime = of.exptime;
            }
            it->it_flags |= ITEM_STALE;
            // Also need to remove TOKEN_SENT, so next client can win.
            it->it_flags &= ~ITEM_TOKEN_SENT;

            ITEM_set_cas(it, (settings.use_cas) ? get_cas_id() : 0);

            // Clients can noreply nominal responses.
            if (c->noreply)
                resp->skip = true;
            memcpy(resp->wbuf, "OK ", 3);
        } else {
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.slab_stats[ITEM_clsid(it)].delete_hits++;
            pthread_mutex_unlock(&c->thread->stats.mutex);

            do_item_unlink(it, hv);
            STORAGE_delete(c->thread->storage, it);
            if (c->noreply)
                resp->skip = true;
            memcpy(resp->wbuf, "OK ", 3);
        }
        goto cleanup;
    } else {
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.delete_misses++;
        pthread_mutex_unlock(&c->thread->stats.mutex);

        memcpy(resp->wbuf, "NF ", 3);
        goto cleanup;
    }
cleanup:
    if (it) {
        do_item_remove(it);
    }
    // Item is always returned locked, even if missing.
    item_unlock(hv);
    resp->wbytes = p - resp->wbuf;
    memcpy(resp->wbuf + resp->wbytes, "\r\n", 2);
    resp->wbytes += 2;
    resp_add_iov(resp, resp->wbuf, resp->wbytes);
    conn_set_state(c, conn_new_cmd);
    return;
error:
    out_errstring(c, errstr);
}