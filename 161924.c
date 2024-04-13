static efi_status_t efi_thunk_get_time(efi_time_t *tm, efi_time_cap_t *tc)
{
	efi_status_t status;
	u32 phys_tm, phys_tc;
	unsigned long flags;

	spin_lock(&rtc_lock);
	spin_lock_irqsave(&efi_runtime_lock, flags);

	phys_tm = virt_to_phys_or_null(tm);
	phys_tc = virt_to_phys_or_null(tc);

	status = efi_thunk(get_time, phys_tm, phys_tc);

	spin_unlock_irqrestore(&efi_runtime_lock, flags);
	spin_unlock(&rtc_lock);

	return status;
}