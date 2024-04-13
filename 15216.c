Napi::Value Statement::Get(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Statement* stmt = this;

    Baton* baton = stmt->Bind<RowBaton>(info);
    if (baton == NULL) {
        Napi::Error::New(env, "Data type is not supported").ThrowAsJavaScriptException();
        return env.Null();
    }
    else {
        stmt->Schedule(Work_BeginGet, baton);
        return info.This();
    }
}