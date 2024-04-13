static void pr_cont_work(bool comma, struct work_struct *work)
{
	if (work->func == wq_barrier_func) {
		struct wq_barrier *barr;

		barr = container_of(work, struct wq_barrier, work);

		pr_cont("%s BAR(%d)", comma ? "," : "",
			task_pid_nr(barr->task));
	} else {
		pr_cont("%s %pf", comma ? "," : "", work->func);
	}
}