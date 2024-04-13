template <class T> void Statement::Error(T* baton) {
    Statement* stmt = baton->stmt;

    Napi::Env env = stmt->Env();
    Napi::HandleScope scope(env);

    // Fail hard on logic errors.
    assert(stmt->status != 0);
    EXCEPTION(Napi::String::New(env, stmt->message.c_str()), stmt->status, exception);

    Napi::Function cb = baton->callback.Value();

    if (!cb.IsUndefined() && cb.IsFunction()) {
        Napi::Value argv[] = { exception };
        TRY_CATCH_CALL(stmt->Value(), cb, 1, argv);
    }
    else {
        Napi::Value argv[] = { Napi::String::New(env, "error"), exception };
        EMIT_EVENT(stmt->Value(), 2, argv);
    }
}