static int __init cmm_init(void)
{
	int rc = -ENOMEM;

	cmm_sysctl_header = register_sysctl_table(cmm_dir_table);
	if (!cmm_sysctl_header)
		goto out_sysctl;
#ifdef CONFIG_CMM_IUCV
	/* convert sender to uppercase characters */
	if (sender) {
		int len = strlen(sender);
		while (len--)
			sender[len] = toupper(sender[len]);
	} else {
		sender = cmm_default_sender;
	}

	rc = smsg_register_callback(SMSG_PREFIX, cmm_smsg_target);
	if (rc < 0)
		goto out_smsg;
#endif
	rc = register_oom_notifier(&cmm_oom_nb);
	if (rc < 0)
		goto out_oom_notify;
	rc = register_pm_notifier(&cmm_power_notifier);
	if (rc)
		goto out_pm;
	cmm_thread_ptr = kthread_run(cmm_thread, NULL, "cmmthread");
	if (!IS_ERR(cmm_thread_ptr))
		return 0;

	rc = PTR_ERR(cmm_thread_ptr);
	unregister_pm_notifier(&cmm_power_notifier);
out_pm:
	unregister_oom_notifier(&cmm_oom_nb);
out_oom_notify:
#ifdef CONFIG_CMM_IUCV
	smsg_unregister_callback(SMSG_PREFIX, cmm_smsg_target);
out_smsg:
#endif
	unregister_sysctl_table(cmm_sysctl_header);
out_sysctl:
	del_timer_sync(&cmm_timer);
	return rc;
}