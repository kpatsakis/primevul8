print_tickdevice(struct seq_file *m, struct tick_device *td, int cpu)
{
	struct clock_event_device *dev = td->evtdev;

	SEQ_printf(m, "Tick Device: mode:     %d\n", td->mode);
	if (cpu < 0)
		SEQ_printf(m, "Broadcast device\n");
	else
		SEQ_printf(m, "Per CPU device: %d\n", cpu);

	SEQ_printf(m, "Clock Event Device: ");
	if (!dev) {
		SEQ_printf(m, "<NULL>\n");
		return;
	}
	SEQ_printf(m, "%s\n", dev->name);
	SEQ_printf(m, " max_delta_ns:   %llu\n",
		   (unsigned long long) dev->max_delta_ns);
	SEQ_printf(m, " min_delta_ns:   %llu\n",
		   (unsigned long long) dev->min_delta_ns);
	SEQ_printf(m, " mult:           %u\n", dev->mult);
	SEQ_printf(m, " shift:          %u\n", dev->shift);
	SEQ_printf(m, " mode:           %d\n", clockevent_get_state(dev));
	SEQ_printf(m, " next_event:     %Ld nsecs\n",
		   (unsigned long long) ktime_to_ns(dev->next_event));

	SEQ_printf(m, " set_next_event: ");
	print_name_offset(m, dev->set_next_event);
	SEQ_printf(m, "\n");

	if (dev->set_state_shutdown) {
		SEQ_printf(m, " shutdown: ");
		print_name_offset(m, dev->set_state_shutdown);
		SEQ_printf(m, "\n");
	}

	if (dev->set_state_periodic) {
		SEQ_printf(m, " periodic: ");
		print_name_offset(m, dev->set_state_periodic);
		SEQ_printf(m, "\n");
	}

	if (dev->set_state_oneshot) {
		SEQ_printf(m, " oneshot:  ");
		print_name_offset(m, dev->set_state_oneshot);
		SEQ_printf(m, "\n");
	}

	if (dev->set_state_oneshot_stopped) {
		SEQ_printf(m, " oneshot stopped: ");
		print_name_offset(m, dev->set_state_oneshot_stopped);
		SEQ_printf(m, "\n");
	}

	if (dev->tick_resume) {
		SEQ_printf(m, " resume:   ");
		print_name_offset(m, dev->tick_resume);
		SEQ_printf(m, "\n");
	}

	SEQ_printf(m, " event_handler:  ");
	print_name_offset(m, dev->event_handler);
	SEQ_printf(m, "\n");
	SEQ_printf(m, " retries:        %lu\n", dev->retries);
	SEQ_printf(m, "\n");
}