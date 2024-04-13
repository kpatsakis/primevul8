static int proc_oom_score(struct task_struct *task, char *buffer)
{
	unsigned long points = 0;

	read_lock(&tasklist_lock);
	if (pid_alive(task))
		points = oom_badness(task, NULL, NULL,
					totalram_pages + total_swap_pages);
	read_unlock(&tasklist_lock);
	return sprintf(buffer, "%lu\n", points);
}