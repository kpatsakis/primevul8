perf_event_groups_next(struct perf_event *event)
{
	struct perf_event *next;
#ifdef CONFIG_CGROUP_PERF
	u64 curr_cgrp_id = 0;
	u64 next_cgrp_id = 0;
#endif

	next = rb_entry_safe(rb_next(&event->group_node), typeof(*event), group_node);
	if (next == NULL || next->cpu != event->cpu)
		return NULL;

#ifdef CONFIG_CGROUP_PERF
	if (event->cgrp && event->cgrp->css.cgroup)
		curr_cgrp_id = event->cgrp->css.cgroup->kn->id;

	if (next->cgrp && next->cgrp->css.cgroup)
		next_cgrp_id = next->cgrp->css.cgroup->kn->id;

	if (curr_cgrp_id != next_cgrp_id)
		return NULL;
#endif
	return next;
}