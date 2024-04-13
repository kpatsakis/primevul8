is_pam_password_change_required(void)
{
	return (sshpam_new_authtok_reqd);
}