void QuotaManager::GetAvailableSpace(AvailableSpaceCallback* callback) {
  scoped_refptr<AvailableSpaceQueryTask> task(
      new AvailableSpaceQueryTask(this, db_thread_, profile_path_, callback));
  task->Start();
}
