Statement::Statement(const Napi::CallbackInfo& info) : Napi::ObjectWrap<Statement>(info) {
    Napi::Env env = info.Env();
    int length = info.Length();

    if (length <= 0 || !Database::HasInstance(info[0])) {
        Napi::TypeError::New(env, "Database object expected").ThrowAsJavaScriptException();
        return;
    }
    else if (length <= 1 || !info[1].IsString()) {
        Napi::TypeError::New(env, "SQL query expected").ThrowAsJavaScriptException();
        return;
    }
    else if (length > 2 && !info[2].IsUndefined() && !info[2].IsFunction()) {
        Napi::TypeError::New(env, "Callback expected").ThrowAsJavaScriptException();
        return;
    }

    Database* db = Napi::ObjectWrap<Database>::Unwrap(info[0].As<Napi::Object>());
    Napi::String sql = info[1].As<Napi::String>();

    info.This().As<Napi::Object>().DefineProperty(Napi::PropertyDescriptor::Value("sql", sql, napi_default));

    init(db);
    Statement* stmt = this;

    PrepareBaton* baton = new PrepareBaton(db, info[2].As<Napi::Function>(), stmt);
    baton->sql = std::string(sql.As<Napi::String>().Utf8Value().c_str());
    db->Schedule(Work_BeginPrepare, baton);
}