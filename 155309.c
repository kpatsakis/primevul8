static void __exit cmm_exit(void)
{
	unregister_sysctl_table(cmm_sysctl_header);
#ifdef CONFIG_CMM_IUCV
	smsg_unregister_callback(SMSG_PREFIX, cmm_smsg_target);
#endif
	unregister_pm_notifier(&cmm_power_notifier);
	unregister_oom_notifier(&cmm_oom_nb);
	kthread_stop(cmm_thread_ptr);
	del_timer_sync(&cmm_timer);
	cmm_free_pages(cmm_pages, &cmm_pages, &cmm_page_list);
	cmm_free_pages(cmm_timed_pages, &cmm_timed_pages, &cmm_timed_page_list);
}