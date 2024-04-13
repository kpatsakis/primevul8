static void wq_watchdog_set_thresh(unsigned long thresh)
{
	wq_watchdog_thresh = 0;
	del_timer_sync(&wq_watchdog_timer);

	if (thresh) {
		wq_watchdog_thresh = thresh;
		wq_watchdog_reset_touched();
		mod_timer(&wq_watchdog_timer, jiffies + thresh * HZ);
	}
}