Napi::Value Statement::All(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Statement* stmt = this;

    Baton* baton = stmt->Bind<RowsBaton>(info);
    if (baton == NULL) {
        Napi::Error::New(env, "Data type is not supported").ThrowAsJavaScriptException();
        return env.Null();
    }
    else {
        stmt->Schedule(Work_BeginAll, baton);
        return info.This();
    }
}