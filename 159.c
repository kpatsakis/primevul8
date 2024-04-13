void QuotaManager::DidGetPersistentGlobalUsageForHistogram(
    StorageType type,
    int64 usage,
    int64 unlimited_usage) {
  UMA_HISTOGRAM_MBYTES("Quota.GlobalUsageOfPersistentStorage", usage);

  std::set<GURL> origins;
  GetCachedOrigins(type, &origins);

  size_t num_origins = origins.size();
  size_t protected_origins = 0;
  size_t unlimited_origins = 0;
  CountOriginType(origins, special_storage_policy_,
                  &protected_origins, &unlimited_origins);

  UMA_HISTOGRAM_COUNTS("Quota.NumberOfPersistentStorageOrigins",
                       num_origins);
  UMA_HISTOGRAM_COUNTS("Quota.NumberOfProtectedPersistentStorageOrigins",
                       protected_origins);
  UMA_HISTOGRAM_COUNTS("Quota.NumberOfUnlimitedPersistentStorageOrigins",
                       unlimited_origins);
}
