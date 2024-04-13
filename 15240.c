void Statement::Work_BeginPrepare(Database::Baton* baton) {
    assert(baton->db->open);
    baton->db->pending++;
    Napi::Env env = baton->db->Env();
    int status = napi_create_async_work(
        env, NULL, Napi::String::New(env, "sqlite3.Statement.Prepare"),
        Work_Prepare, Work_AfterPrepare, baton, &baton->request
    );
    assert(status == 0);
    napi_queue_async_work(env, baton->request);
}