void QuotaManager::DumpOriginInfoTable(
    DumpOriginInfoTableCallback* callback) {
  make_scoped_refptr(new DumpOriginInfoTableTask(this, callback))->Start();
}
