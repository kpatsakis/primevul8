void Statement::Work_Each(napi_env e, void* data) {
    STATEMENT_INIT(EachBaton);

    Async* async = baton->async;

    STATEMENT_MUTEX(mtx);

    int retrieved = 0;

    // Make sure that we also reset when there are no parameters.
    if (!baton->parameters.size()) {
        sqlite3_reset(stmt->_handle);
    }

    if (stmt->Bind(baton->parameters)) {
        while (true) {
            sqlite3_mutex_enter(mtx);
            stmt->status = sqlite3_step(stmt->_handle);
            if (stmt->status == SQLITE_ROW) {
                sqlite3_mutex_leave(mtx);
                Row* row = new Row();
                GetRow(row, stmt->_handle);
                NODE_SQLITE3_MUTEX_LOCK(&async->mutex)
                async->data.push_back(row);
                retrieved++;
                NODE_SQLITE3_MUTEX_UNLOCK(&async->mutex)

                uv_async_send(&async->watcher);
            }
            else {
                if (stmt->status != SQLITE_DONE) {
                    stmt->message = std::string(sqlite3_errmsg(stmt->db->_handle));
                }
                sqlite3_mutex_leave(mtx);
                break;
            }
        }
    }

    async->completed = true;
    uv_async_send(&async->watcher);
}