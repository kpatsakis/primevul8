void efi_switch_mm(struct mm_struct *mm)
{
	efi_scratch.prev_mm = current->active_mm;
	current->active_mm = mm;
	switch_mm(efi_scratch.prev_mm, mm, NULL);
}