static bool resp_start(conn *c) {
    mc_resp *resp = do_cache_alloc(c->thread->resp_cache);
    if (!resp) {
        THR_STATS_LOCK(c);
        c->thread->stats.response_obj_oom++;
        THR_STATS_UNLOCK(c);
        return false;
    }
    // FIXME: make wbuf indirect or use offsetof to zero up until wbuf
    memset(resp, 0, sizeof(*resp));
    if (!c->resp_head) {
        c->resp_head = resp;
    }
    if (!c->resp) {
        c->resp = resp;
    } else {
        c->resp->next = resp;
        c->resp = resp;
    }
    if (IS_UDP(c->transport)) {
        // need to hold on to some data for async responses.
        c->resp->request_id = c->request_id;
        c->resp->request_addr = c->request_addr;
        c->resp->request_addr_size = c->request_addr_size;
    }
    return true;
}