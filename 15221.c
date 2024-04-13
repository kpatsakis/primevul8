void Statement::Work_Get(napi_env e, void* data) {
    STATEMENT_INIT(RowBaton);

    if (stmt->status != SQLITE_DONE || baton->parameters.size()) {
        STATEMENT_MUTEX(mtx);
        sqlite3_mutex_enter(mtx);

        if (stmt->Bind(baton->parameters)) {
            stmt->status = sqlite3_step(stmt->_handle);

            if (!(stmt->status == SQLITE_ROW || stmt->status == SQLITE_DONE)) {
                stmt->message = std::string(sqlite3_errmsg(stmt->db->_handle));
            }
        }

        sqlite3_mutex_leave(mtx);

        if (stmt->status == SQLITE_ROW) {
            // Acquire one result row before returning.
            GetRow(&baton->row, stmt->_handle);
        }
    }
}