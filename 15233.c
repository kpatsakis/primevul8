void Statement::Work_AfterRun(napi_env e, napi_status status, void* data) {
    std::unique_ptr<RunBaton> baton(static_cast<RunBaton*>(data));
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
            (stmt->Value()).Set(Napi::String::New(env, "lastID"), Napi::Number::New(env, baton->inserted_id));
            (stmt->Value()).Set( Napi::String::New(env, "changes"), Napi::Number::New(env, baton->changes));

            Napi::Value argv[] = { env.Null() };
            TRY_CATCH_CALL(stmt->Value(), cb, 1, argv);
        }
    }

    STATEMENT_END();
}