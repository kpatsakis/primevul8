Napi::Value Statement::Reset(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Statement* stmt = this;

    OPTIONAL_ARGUMENT_FUNCTION(0, callback);

    Baton* baton = new Baton(stmt, callback);
    stmt->Schedule(Work_BeginReset, baton);

    return info.This();
}