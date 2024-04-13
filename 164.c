void QuotaManager::DumpQuotaTable(DumpQuotaTableCallback* callback) {
  make_scoped_refptr(new DumpQuotaTableTask(this, callback))->Start();
}
