perf_event_groups_insert(struct perf_event_groups *groups,
			 struct perf_event *event)
{
	struct perf_event *node_event;
	struct rb_node *parent;
	struct rb_node **node;

	event->group_index = ++groups->index;

	node = &groups->tree.rb_node;
	parent = *node;

	while (*node) {
		parent = *node;
		node_event = container_of(*node, struct perf_event, group_node);

		if (perf_event_groups_less(event, node_event))
			node = &parent->rb_left;
		else
			node = &parent->rb_right;
	}

	rb_link_node(&event->group_node, parent, node);
	rb_insert_color(&event->group_node, &groups->tree);
}