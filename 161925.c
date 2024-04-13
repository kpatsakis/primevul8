void efi_thunk_runtime_setup(void)
{
	efi.get_time = efi_thunk_get_time;
	efi.set_time = efi_thunk_set_time;
	efi.get_wakeup_time = efi_thunk_get_wakeup_time;
	efi.set_wakeup_time = efi_thunk_set_wakeup_time;
	efi.get_variable = efi_thunk_get_variable;
	efi.get_next_variable = efi_thunk_get_next_variable;
	efi.set_variable = efi_thunk_set_variable;
	efi.set_variable_nonblocking = efi_thunk_set_variable_nonblocking;
	efi.get_next_high_mono_count = efi_thunk_get_next_high_mono_count;
	efi.reset_system = efi_thunk_reset_system;
	efi.query_variable_info = efi_thunk_query_variable_info;
	efi.query_variable_info_nonblocking = efi_thunk_query_variable_info_nonblocking;
	efi.update_capsule = efi_thunk_update_capsule;
	efi.query_capsule_caps = efi_thunk_query_capsule_caps;
}