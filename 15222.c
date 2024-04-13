void Statement::Work_AfterReset(napi_env e, napi_status status, void* data) {
    std::unique_ptr<Baton> baton(static_cast<Baton*>(data));
    Statement* stmt = baton->stmt;

    Napi::Env env = stmt->Env();
    Napi::HandleScope scope(env);

    // Fire callbacks.
    Napi::Function cb = baton->callback.Value();
    if (!cb.IsUndefined() && cb.IsFunction()) {
        Napi::Value argv[] = { env.Null() };
        TRY_CATCH_CALL(stmt->Value(), cb, 1, argv);
    }

    STATEMENT_END();
}