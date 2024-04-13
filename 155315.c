static int cmm_thread(void *dummy)
{
	int rc;

	while (1) {
		rc = wait_event_interruptible(cmm_thread_wait,
			(!cmm_suspended && (cmm_pages != cmm_pages_target ||
			 cmm_timed_pages != cmm_timed_pages_target)) ||
			 kthread_should_stop());
		if (kthread_should_stop() || rc == -ERESTARTSYS) {
			cmm_pages_target = cmm_pages;
			cmm_timed_pages_target = cmm_timed_pages;
			break;
		}
		if (cmm_pages_target > cmm_pages) {
			if (cmm_alloc_pages(1, &cmm_pages, &cmm_page_list))
				cmm_pages_target = cmm_pages;
		} else if (cmm_pages_target < cmm_pages) {
			cmm_free_pages(1, &cmm_pages, &cmm_page_list);
		}
		if (cmm_timed_pages_target > cmm_timed_pages) {
			if (cmm_alloc_pages(1, &cmm_timed_pages,
					   &cmm_timed_page_list))
				cmm_timed_pages_target = cmm_timed_pages;
		} else if (cmm_timed_pages_target < cmm_timed_pages) {
			cmm_free_pages(1, &cmm_timed_pages,
				       &cmm_timed_page_list);
		}
		if (cmm_timed_pages > 0 && !timer_pending(&cmm_timer))
			cmm_set_timer();
	}
	return 0;
}