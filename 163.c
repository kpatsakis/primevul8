  DumpOriginInfoTableTask(
      QuotaManager* manager,
      Callback* callback)
      : DatabaseTaskBase(manager),
        callback_(callback) {
  }
