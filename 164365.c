static void server_stats(ADD_STAT add_stats, conn *c) {
    pid_t pid = getpid();
    rel_time_t now = current_time;

    struct thread_stats thread_stats;
    threadlocal_stats_aggregate(&thread_stats);
    struct slab_stats slab_stats;
    slab_stats_aggregate(&thread_stats, &slab_stats);
#ifdef EXTSTORE
    struct extstore_stats st;
#endif
#ifndef WIN32
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
#endif /* !WIN32 */

    STATS_LOCK();

    APPEND_STAT("pid", "%lu", (long)pid);
    APPEND_STAT("uptime", "%u", now - ITEM_UPDATE_INTERVAL);
    APPEND_STAT("time", "%ld", now + (long)process_started);
    APPEND_STAT("version", "%s", VERSION);
    APPEND_STAT("libevent", "%s", event_get_version());
    APPEND_STAT("pointer_size", "%d", (int)(8 * sizeof(void *)));

#ifndef WIN32
    append_stat("rusage_user", add_stats, c, "%ld.%06ld",
                (long)usage.ru_utime.tv_sec,
                (long)usage.ru_utime.tv_usec);
    append_stat("rusage_system", add_stats, c, "%ld.%06ld",
                (long)usage.ru_stime.tv_sec,
                (long)usage.ru_stime.tv_usec);
#endif /* !WIN32 */

    APPEND_STAT("max_connections", "%d", settings.maxconns);
    APPEND_STAT("curr_connections", "%llu", (unsigned long long)stats_state.curr_conns - 1);
    APPEND_STAT("total_connections", "%llu", (unsigned long long)stats.total_conns);
    if (settings.maxconns_fast) {
        APPEND_STAT("rejected_connections", "%llu", (unsigned long long)stats.rejected_conns);
    }
    APPEND_STAT("connection_structures", "%u", stats_state.conn_structs);
    APPEND_STAT("response_obj_bytes", "%llu", (unsigned long long)thread_stats.response_obj_bytes);
    APPEND_STAT("response_obj_total", "%llu", (unsigned long long)thread_stats.response_obj_total);
    APPEND_STAT("response_obj_free", "%llu", (unsigned long long)thread_stats.response_obj_free);
    APPEND_STAT("response_obj_oom", "%llu", (unsigned long long)thread_stats.response_obj_oom);
    APPEND_STAT("read_buf_bytes", "%llu", (unsigned long long)thread_stats.read_buf_bytes);
    APPEND_STAT("read_buf_bytes_free", "%llu", (unsigned long long)thread_stats.read_buf_bytes_free);
    APPEND_STAT("read_buf_oom", "%llu", (unsigned long long)thread_stats.read_buf_oom);
    APPEND_STAT("reserved_fds", "%u", stats_state.reserved_fds);
    APPEND_STAT("cmd_get", "%llu", (unsigned long long)thread_stats.get_cmds);
    APPEND_STAT("cmd_set", "%llu", (unsigned long long)slab_stats.set_cmds);
    APPEND_STAT("cmd_flush", "%llu", (unsigned long long)thread_stats.flush_cmds);
    APPEND_STAT("cmd_touch", "%llu", (unsigned long long)thread_stats.touch_cmds);
    APPEND_STAT("cmd_meta", "%llu", (unsigned long long)thread_stats.meta_cmds);
    APPEND_STAT("get_hits", "%llu", (unsigned long long)slab_stats.get_hits);
    APPEND_STAT("get_misses", "%llu", (unsigned long long)thread_stats.get_misses);
    APPEND_STAT("get_expired", "%llu", (unsigned long long)thread_stats.get_expired);
    APPEND_STAT("get_flushed", "%llu", (unsigned long long)thread_stats.get_flushed);
#ifdef EXTSTORE
    if (c->thread->storage) {
        APPEND_STAT("get_extstore", "%llu", (unsigned long long)thread_stats.get_extstore);
        APPEND_STAT("get_aborted_extstore", "%llu", (unsigned long long)thread_stats.get_aborted_extstore);
        APPEND_STAT("get_oom_extstore", "%llu", (unsigned long long)thread_stats.get_oom_extstore);
        APPEND_STAT("recache_from_extstore", "%llu", (unsigned long long)thread_stats.recache_from_extstore);
        APPEND_STAT("miss_from_extstore", "%llu", (unsigned long long)thread_stats.miss_from_extstore);
        APPEND_STAT("badcrc_from_extstore", "%llu", (unsigned long long)thread_stats.badcrc_from_extstore);
    }
#endif
    APPEND_STAT("delete_misses", "%llu", (unsigned long long)thread_stats.delete_misses);
    APPEND_STAT("delete_hits", "%llu", (unsigned long long)slab_stats.delete_hits);
    APPEND_STAT("incr_misses", "%llu", (unsigned long long)thread_stats.incr_misses);
    APPEND_STAT("incr_hits", "%llu", (unsigned long long)slab_stats.incr_hits);
    APPEND_STAT("decr_misses", "%llu", (unsigned long long)thread_stats.decr_misses);
    APPEND_STAT("decr_hits", "%llu", (unsigned long long)slab_stats.decr_hits);
    APPEND_STAT("cas_misses", "%llu", (unsigned long long)thread_stats.cas_misses);
    APPEND_STAT("cas_hits", "%llu", (unsigned long long)slab_stats.cas_hits);
    APPEND_STAT("cas_badval", "%llu", (unsigned long long)slab_stats.cas_badval);
    APPEND_STAT("touch_hits", "%llu", (unsigned long long)slab_stats.touch_hits);
    APPEND_STAT("touch_misses", "%llu", (unsigned long long)thread_stats.touch_misses);
    APPEND_STAT("auth_cmds", "%llu", (unsigned long long)thread_stats.auth_cmds);
    APPEND_STAT("auth_errors", "%llu", (unsigned long long)thread_stats.auth_errors);
    if (settings.idle_timeout) {
        APPEND_STAT("idle_kicks", "%llu", (unsigned long long)thread_stats.idle_kicks);
    }
    APPEND_STAT("bytes_read", "%llu", (unsigned long long)thread_stats.bytes_read);
    APPEND_STAT("bytes_written", "%llu", (unsigned long long)thread_stats.bytes_written);
    APPEND_STAT("limit_maxbytes", "%llu", (unsigned long long)settings.maxbytes);
    APPEND_STAT("accepting_conns", "%u", stats_state.accepting_conns);
    APPEND_STAT("listen_disabled_num", "%llu", (unsigned long long)stats.listen_disabled_num);
    APPEND_STAT("time_in_listen_disabled_us", "%llu", stats.time_in_listen_disabled_us);
    APPEND_STAT("threads", "%d", settings.num_threads);
    APPEND_STAT("conn_yields", "%llu", (unsigned long long)thread_stats.conn_yields);
    APPEND_STAT("hash_power_level", "%u", stats_state.hash_power_level);
    APPEND_STAT("hash_bytes", "%llu", (unsigned long long)stats_state.hash_bytes);
    APPEND_STAT("hash_is_expanding", "%u", stats_state.hash_is_expanding);
    if (settings.slab_reassign) {
        APPEND_STAT("slab_reassign_rescues", "%llu", stats.slab_reassign_rescues);
        APPEND_STAT("slab_reassign_chunk_rescues", "%llu", stats.slab_reassign_chunk_rescues);
        APPEND_STAT("slab_reassign_evictions_nomem", "%llu", stats.slab_reassign_evictions_nomem);
        APPEND_STAT("slab_reassign_inline_reclaim", "%llu", stats.slab_reassign_inline_reclaim);
        APPEND_STAT("slab_reassign_busy_items", "%llu", stats.slab_reassign_busy_items);
        APPEND_STAT("slab_reassign_busy_deletes", "%llu", stats.slab_reassign_busy_deletes);
        APPEND_STAT("slab_reassign_running", "%u", stats_state.slab_reassign_running);
        APPEND_STAT("slabs_moved", "%llu", stats.slabs_moved);
    }
    if (settings.lru_crawler) {
        APPEND_STAT("lru_crawler_running", "%u", stats_state.lru_crawler_running);
        APPEND_STAT("lru_crawler_starts", "%u", stats.lru_crawler_starts);
    }
    if (settings.lru_maintainer_thread) {
        APPEND_STAT("lru_maintainer_juggles", "%llu", (unsigned long long)stats.lru_maintainer_juggles);
    }
    APPEND_STAT("malloc_fails", "%llu",
                (unsigned long long)stats.malloc_fails);
    APPEND_STAT("log_worker_dropped", "%llu", (unsigned long long)stats.log_worker_dropped);
    APPEND_STAT("log_worker_written", "%llu", (unsigned long long)stats.log_worker_written);
    APPEND_STAT("log_watcher_skipped", "%llu", (unsigned long long)stats.log_watcher_skipped);
    APPEND_STAT("log_watcher_sent", "%llu", (unsigned long long)stats.log_watcher_sent);
    STATS_UNLOCK();
#ifdef EXTSTORE
    if (c->thread->storage) {
        STATS_LOCK();
        APPEND_STAT("extstore_compact_lost", "%llu", (unsigned long long)stats.extstore_compact_lost);
        APPEND_STAT("extstore_compact_rescues", "%llu", (unsigned long long)stats.extstore_compact_rescues);
        APPEND_STAT("extstore_compact_skipped", "%llu", (unsigned long long)stats.extstore_compact_skipped);
        STATS_UNLOCK();
        extstore_get_stats(c->thread->storage, &st);
        APPEND_STAT("extstore_page_allocs", "%llu", (unsigned long long)st.page_allocs);
        APPEND_STAT("extstore_page_evictions", "%llu", (unsigned long long)st.page_evictions);
        APPEND_STAT("extstore_page_reclaims", "%llu", (unsigned long long)st.page_reclaims);
        APPEND_STAT("extstore_pages_free", "%llu", (unsigned long long)st.pages_free);
        APPEND_STAT("extstore_pages_used", "%llu", (unsigned long long)st.pages_used);
        APPEND_STAT("extstore_objects_evicted", "%llu", (unsigned long long)st.objects_evicted);
        APPEND_STAT("extstore_objects_read", "%llu", (unsigned long long)st.objects_read);
        APPEND_STAT("extstore_objects_written", "%llu", (unsigned long long)st.objects_written);
        APPEND_STAT("extstore_objects_used", "%llu", (unsigned long long)st.objects_used);
        APPEND_STAT("extstore_bytes_evicted", "%llu", (unsigned long long)st.bytes_evicted);
        APPEND_STAT("extstore_bytes_written", "%llu", (unsigned long long)st.bytes_written);
        APPEND_STAT("extstore_bytes_read", "%llu", (unsigned long long)st.bytes_read);
        APPEND_STAT("extstore_bytes_used", "%llu", (unsigned long long)st.bytes_used);
        APPEND_STAT("extstore_bytes_fragmented", "%llu", (unsigned long long)st.bytes_fragmented);
        APPEND_STAT("extstore_limit_maxbytes", "%llu", (unsigned long long)(st.page_count * st.page_size));
        APPEND_STAT("extstore_io_queue", "%llu", (unsigned long long)(st.io_queue));
    }
#endif
#ifdef TLS
    if (settings.ssl_enabled) {
        APPEND_STAT("ssl_handshake_errors", "%llu", (unsigned long long)stats.ssl_handshake_errors);
        APPEND_STAT("time_since_server_cert_refresh", "%u", now - settings.ssl_last_cert_refresh_time);
    }
#endif
}