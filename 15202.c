void Statement::Finalize_() {
    assert(!finalized);
    finalized = true;
    CleanQueue();
    // Finalize returns the status code of the last operation. We already fired
    // error events in case those failed.
    sqlite3_finalize(_handle);
    _handle = NULL;
    db->Unref();
}