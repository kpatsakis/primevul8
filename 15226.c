void Statement::Work_AfterEach(napi_env e, napi_status status, void* data) {
    std::unique_ptr<EachBaton> baton(static_cast<EachBaton*>(data));
    Statement* stmt = baton->stmt;

    Napi::Env env = stmt->Env();
    Napi::HandleScope scope(env);

    if (stmt->status != SQLITE_DONE) {
        Error(baton.get());
    }

    STATEMENT_END();
}