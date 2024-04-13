void nego_set_restricted_admin_mode_required(rdpNego* nego, BOOL RestrictedAdminModeRequired)
{
	WLog_DBG(TAG, "Enabling restricted admin mode: %s",
	         RestrictedAdminModeRequired ? "TRUE" : "FALSE");
	nego->RestrictedAdminModeRequired = RestrictedAdminModeRequired;
}