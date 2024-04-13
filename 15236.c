void Statement::Work_AfterGet(napi_env e, napi_status status, void* data) {
    std::unique_ptr<RowBaton> baton(static_cast<RowBaton*>(data));
    Statement* stmt = baton->stmt;

    Napi::Env env = stmt->Env();
    Napi::HandleScope scope(env);

    if (stmt->status != SQLITE_ROW && stmt->status != SQLITE_DONE) {
        Error(baton.get());
    }
    else {
        // Fire callbacks.
        Napi::Function cb = baton->callback.Value();
        if (!cb.IsUndefined() && cb.IsFunction()) {
            if (stmt->status == SQLITE_ROW) {
                // Create the result array from the data we acquired.
                Napi::Value argv[] = { env.Null(), RowToJS(env, &baton->row) };
                TRY_CATCH_CALL(stmt->Value(), cb, 2, argv);
            }
            else {
                Napi::Value argv[] = { env.Null() };
                TRY_CATCH_CALL(stmt->Value(), cb, 1, argv);
            }
        }
    }

    STATEMENT_END();
}