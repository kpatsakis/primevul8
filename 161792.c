static void show_pwq(struct pool_workqueue *pwq)
{
	struct worker_pool *pool = pwq->pool;
	struct work_struct *work;
	struct worker *worker;
	bool has_in_flight = false, has_pending = false;
	int bkt;

	pr_info("  pwq %d:", pool->id);
	pr_cont_pool_info(pool);

	pr_cont(" active=%d/%d%s\n", pwq->nr_active, pwq->max_active,
		!list_empty(&pwq->mayday_node) ? " MAYDAY" : "");

	hash_for_each(pool->busy_hash, bkt, worker, hentry) {
		if (worker->current_pwq == pwq) {
			has_in_flight = true;
			break;
		}
	}
	if (has_in_flight) {
		bool comma = false;

		pr_info("    in-flight:");
		hash_for_each(pool->busy_hash, bkt, worker, hentry) {
			if (worker->current_pwq != pwq)
				continue;

			pr_cont("%s %d%s:%pf", comma ? "," : "",
				task_pid_nr(worker->task),
				worker == pwq->wq->rescuer ? "(RESCUER)" : "",
				worker->current_func);
			list_for_each_entry(work, &worker->scheduled, entry)
				pr_cont_work(false, work);
			comma = true;
		}
		pr_cont("\n");
	}

	list_for_each_entry(work, &pool->worklist, entry) {
		if (get_work_pwq(work) == pwq) {
			has_pending = true;
			break;
		}
	}
	if (has_pending) {
		bool comma = false;

		pr_info("    pending:");
		list_for_each_entry(work, &pool->worklist, entry) {
			if (get_work_pwq(work) != pwq)
				continue;

			pr_cont_work(comma, work);
			comma = !(*work_data_bits(work) & WORK_STRUCT_LINKED);
		}
		pr_cont("\n");
	}

	if (!list_empty(&pwq->delayed_works)) {
		bool comma = false;

		pr_info("    delayed:");
		list_for_each_entry(work, &pwq->delayed_works, entry) {
			pr_cont_work(comma, work);
			comma = !(*work_data_bits(work) & WORK_STRUCT_LINKED);
		}
		pr_cont("\n");
	}
}