efi_thunk_get_wakeup_time(efi_bool_t *enabled, efi_bool_t *pending,
			  efi_time_t *tm)
{
	efi_status_t status;
	u32 phys_enabled, phys_pending, phys_tm;
	unsigned long flags;

	spin_lock(&rtc_lock);
	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_enabled = virt_to_phys_or_null(enabled);
	phys_pending = virt_to_phys_or_null(pending);
	phys_tm = virt_to_phys_or_null(tm);

	status = efi_thunk(get_wakeup_time, phys_enabled,
			     phys_pending, phys_tm);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);
	spin_unlock(&rtc_lock);

	return status;
}