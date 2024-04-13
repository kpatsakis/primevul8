static int cmm_power_event(struct notifier_block *this,
			   unsigned long event, void *ptr)
{
	switch (event) {
	case PM_POST_HIBERNATION:
		return cmm_resume();
	case PM_HIBERNATION_PREPARE:
		return cmm_suspend();
	default:
		return NOTIFY_DONE;
	}
}