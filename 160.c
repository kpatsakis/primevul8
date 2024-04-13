void QuotaManager::DidInitializeTemporaryGlobalQuota(int64 quota) {
  temporary_global_quota_ = quota;
  temporary_global_quota_callbacks_.Run(
      db_disabled_ ? kQuotaErrorInvalidAccess : kQuotaStatusOk,
      kStorageTypeTemporary, quota);

  if (db_disabled_ || eviction_disabled_)
    return;

  if (!need_initialize_origins_) {
    StartEviction();
    return;
  }

  temporary_usage_tracker_->GetGlobalUsage(callback_factory_.NewCallback(
      &QuotaManager::DidRunInitialGetTemporaryGlobalUsage));
}
