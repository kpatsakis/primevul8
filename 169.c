void QuotaManager::GetGlobalUsage(
    StorageType type,
    GlobalUsageCallback* callback) {
  LazyInitialize();
  GetUsageTracker(type)->GetGlobalUsage(callback);
}
