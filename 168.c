void QuotaManager::GetCachedOrigins(
    StorageType type, std::set<GURL>* origins) {
  DCHECK(origins);
  LazyInitialize();
  switch (type) {
    case kStorageTypeTemporary:
      DCHECK(temporary_usage_tracker_.get());
      temporary_usage_tracker_->GetCachedOrigins(origins);
      return;
    case kStorageTypePersistent:
      DCHECK(persistent_usage_tracker_.get());
      persistent_usage_tracker_->GetCachedOrigins(origins);
      return;
    default:
      NOTREACHED();
  }
}
