Napi::Object Statement::Init(Napi::Env env, Napi::Object exports) {
    Napi::HandleScope scope(env);

    Napi::Function t = DefineClass(env, "Statement", {
      InstanceMethod("bind", &Statement::Bind),
      InstanceMethod("get", &Statement::Get),
      InstanceMethod("run", &Statement::Run),
      InstanceMethod("all", &Statement::All),
      InstanceMethod("each", &Statement::Each),
      InstanceMethod("reset", &Statement::Reset),
      InstanceMethod("finalize", &Statement::Finalize_),
    });

    exports.Set("Statement", t);
    return exports;
}