efi_thunk_set_wakeup_time(efi_bool_t enabled, efi_time_t *tm)
{
	efi_status_t status;
	u32 phys_tm;
	unsigned long flags;

	spin_lock(&rtc_lock);
	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_tm = virt_to_phys_or_null(tm);

	status = efi_thunk(set_wakeup_time, enabled, phys_tm);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);
	spin_unlock(&rtc_lock);

	return status;
}