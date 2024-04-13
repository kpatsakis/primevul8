void Statement::Finalize_(Baton* b) {
    std::unique_ptr<Baton> baton(b);
    Napi::Env env = baton->stmt->Env();
    Napi::HandleScope scope(env);

    baton->stmt->Finalize_();

    // Fire callback in case there was one.
    Napi::Function cb = baton->callback.Value();
    if (!cb.IsUndefined() && cb.IsFunction()) {
        TRY_CATCH_CALL(baton->stmt->Value(), cb, 0, NULL);
    }
}