enum store_item_type do_store_item(item *it, int comm, conn *c, const uint32_t hv) {
    char *key = ITEM_key(it);
    item *old_it = do_item_get(key, it->nkey, hv, c, DONT_UPDATE);
    enum store_item_type stored = NOT_STORED;

    enum cas_result { CAS_NONE, CAS_MATCH, CAS_BADVAL, CAS_STALE, CAS_MISS };

    item *new_it = NULL;
    uint32_t flags;

    /* Do the CAS test up front so we can apply to all store modes */
    enum cas_result cas_res = CAS_NONE;

    bool do_store = false;
    if (old_it != NULL) {
        // Most of the CAS work requires something to compare to.
        uint64_t it_cas = ITEM_get_cas(it);
        uint64_t old_cas = ITEM_get_cas(old_it);
        if (it_cas == 0) {
            cas_res = CAS_NONE;
        } else if (it_cas == old_cas) {
            cas_res = CAS_MATCH;
        } else if (c->set_stale && it_cas < old_cas) {
            cas_res = CAS_STALE;
        } else {
            cas_res = CAS_BADVAL;
        }

        switch (comm) {
            case NREAD_ADD:
                /* add only adds a nonexistent item, but promote to head of LRU */
                do_item_update(old_it);
                break;
            case NREAD_CAS:
                if (cas_res == CAS_MATCH) {
                    // cas validates
                    // it and old_it may belong to different classes.
                    // I'm updating the stats for the one that's getting pushed out
                    pthread_mutex_lock(&c->thread->stats.mutex);
                    c->thread->stats.slab_stats[ITEM_clsid(old_it)].cas_hits++;
                    pthread_mutex_unlock(&c->thread->stats.mutex);
                    do_store = true;
                } else if (cas_res == CAS_STALE) {
                    // if we're allowed to set a stale value, CAS must be lower than
                    // the current item's CAS.
                    // This replaces the value, but should preserve TTL, and stale
                    // item marker bit + token sent if exists.
                    it->exptime = old_it->exptime;
                    it->it_flags |= ITEM_STALE;
                    if (old_it->it_flags & ITEM_TOKEN_SENT) {
                        it->it_flags |= ITEM_TOKEN_SENT;
                    }

                    pthread_mutex_lock(&c->thread->stats.mutex);
                    c->thread->stats.slab_stats[ITEM_clsid(old_it)].cas_hits++;
                    pthread_mutex_unlock(&c->thread->stats.mutex);
                    do_store = true;
                } else {
                    // NONE or BADVAL are the same for CAS cmd
                    pthread_mutex_lock(&c->thread->stats.mutex);
                    c->thread->stats.slab_stats[ITEM_clsid(old_it)].cas_badval++;
                    pthread_mutex_unlock(&c->thread->stats.mutex);

                    if (settings.verbose > 1) {
                        fprintf(stderr, "CAS:  failure: expected %llu, got %llu\n",
                                (unsigned long long)ITEM_get_cas(old_it),
                                (unsigned long long)ITEM_get_cas(it));
                    }
                    stored = EXISTS;
                }
                break;
            case NREAD_APPEND:
            case NREAD_PREPEND:
                if (cas_res != CAS_NONE && cas_res != CAS_MATCH) {
                    stored = EXISTS;
                    break;
                }
#ifdef EXTSTORE
                if ((old_it->it_flags & ITEM_HDR) != 0) {
                    /* block append/prepend from working with extstore-d items.
                     * leave response code to NOT_STORED default */
                    break;
                }
#endif
                /* we have it and old_it here - alloc memory to hold both */
                FLAGS_CONV(old_it, flags);
                new_it = do_item_alloc(key, it->nkey, flags, old_it->exptime, it->nbytes + old_it->nbytes - 2 /* CRLF */);

                // OOM trying to copy.
                if (new_it == NULL)
                    break;
                /* copy data from it and old_it to new_it */
                if (_store_item_copy_data(comm, old_it, new_it, it) == -1) {
                    // failed data copy
                    break;
                } else {
                    // refcount of new_it is 1 here. will end up 2 after link.
                    // it's original ref is managed outside of this function
                    it = new_it;
                    do_store = true;
                }
                break;
            case NREAD_REPLACE:
            case NREAD_SET:
                do_store = true;
                break;
        }

        if (do_store) {
            STORAGE_delete(c->thread->storage, old_it);
            item_replace(old_it, it, hv);
            stored = STORED;
        }

        do_item_remove(old_it);         /* release our reference */
        if (new_it != NULL) {
            // append/prepend end up with an extra reference for new_it.
            do_item_remove(new_it);
        }
    } else {
        /* No pre-existing item to replace or compare to. */
        if (ITEM_get_cas(it) != 0) {
            /* Asked for a CAS match but nothing to compare it to. */
            cas_res = CAS_MISS;
        }

        switch (comm) {
            case NREAD_ADD:
            case NREAD_SET:
                do_store = true;
                break;
            case NREAD_CAS:
                // LRU expired
                stored = NOT_FOUND;
                pthread_mutex_lock(&c->thread->stats.mutex);
                c->thread->stats.cas_misses++;
                pthread_mutex_unlock(&c->thread->stats.mutex);
                break;
            case NREAD_REPLACE:
            case NREAD_APPEND:
            case NREAD_PREPEND:
                /* Requires an existing item. */
                break;
        }

        if (do_store) {
            do_item_link(it, hv);
            stored = STORED;
        }
    }

    if (stored == STORED) {
        c->cas = ITEM_get_cas(it);
    }
    LOGGER_LOG(c->thread->l, LOG_MUTATIONS, LOGGER_ITEM_STORE, NULL,
            stored, comm, ITEM_key(it), it->nkey, it->exptime, ITEM_clsid(it), c->sfd);

    return stored;
}