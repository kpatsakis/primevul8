bool current_is_workqueue_rescuer(void)
{
	struct worker *worker = current_wq_worker();

	return worker && worker->rescue_wq;
}