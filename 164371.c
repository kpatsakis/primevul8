void conn_close_all(void) {
    int i;
    for (i = 0; i < max_fds; i++) {
        if (conns[i] && conns[i]->state != conn_closed) {
            conn_close(conns[i]);
        }
    }
}