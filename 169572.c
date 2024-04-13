perf_event_groups_first(struct perf_event_groups *groups, int cpu,
			struct cgroup *cgrp)
{
	struct perf_event *node_event = NULL, *match = NULL;
	struct rb_node *node = groups->tree.rb_node;
#ifdef CONFIG_CGROUP_PERF
	u64 node_cgrp_id, cgrp_id = 0;

	if (cgrp)
		cgrp_id = cgrp->kn->id;
#endif

	while (node) {
		node_event = container_of(node, struct perf_event, group_node);

		if (cpu < node_event->cpu) {
			node = node->rb_left;
			continue;
		}
		if (cpu > node_event->cpu) {
			node = node->rb_right;
			continue;
		}
#ifdef CONFIG_CGROUP_PERF
		node_cgrp_id = 0;
		if (node_event->cgrp && node_event->cgrp->css.cgroup)
			node_cgrp_id = node_event->cgrp->css.cgroup->kn->id;

		if (cgrp_id < node_cgrp_id) {
			node = node->rb_left;
			continue;
		}
		if (cgrp_id > node_cgrp_id) {
			node = node->rb_right;
			continue;
		}
#endif
		match = node_event;
		node = node->rb_left;
	}

	return match;
}