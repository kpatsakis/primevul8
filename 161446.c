void clock_was_set_delayed(void)
{
	schedule_work(&hrtimer_work);
}