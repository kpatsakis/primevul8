Napi::Value Statement::Bind(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Statement* stmt = this;

    Baton* baton = stmt->Bind<Baton>(info);
    if (baton == NULL) {
        Napi::TypeError::New(env, "Data type is not supported").ThrowAsJavaScriptException();
        return env.Null();
    }
    else {
        stmt->Schedule(Work_BeginBind, baton);
        return info.This();
    }
}