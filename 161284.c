int set_security_override(struct cred *new, u32 secid)
{
	return security_kernel_act_as(new, secid);
}