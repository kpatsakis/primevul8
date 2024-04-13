static void fn_SAK(struct vc_data *vc)
{
	struct work_struct *SAK_work = &vc_cons[fg_console].SAK_work;
	schedule_work(SAK_work);
}