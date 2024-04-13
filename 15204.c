void Statement::Work_All(napi_env e, void* data) {
    STATEMENT_INIT(RowsBaton);

    STATEMENT_MUTEX(mtx);
    sqlite3_mutex_enter(mtx);

    // Make sure that we also reset when there are no parameters.
    if (!baton->parameters.size()) {
        sqlite3_reset(stmt->_handle);
    }

    if (stmt->Bind(baton->parameters)) {
        while ((stmt->status = sqlite3_step(stmt->_handle)) == SQLITE_ROW) {
            Row* row = new Row();
            GetRow(row, stmt->_handle);
            baton->rows.push_back(row);
        }

        if (stmt->status != SQLITE_DONE) {
            stmt->message = std::string(sqlite3_errmsg(stmt->db->_handle));
        }
    }

    sqlite3_mutex_leave(mtx);
}