static void __evdev_flush_queue(struct evdev_client *client, unsigned int type)
{
	unsigned int i, head, num;
	unsigned int mask = client->bufsize - 1;
	bool is_report;
	struct input_event *ev;

	BUG_ON(type == EV_SYN);

	head = client->tail;
	client->packet_head = client->tail;

	/* init to 1 so a leading SYN_REPORT will not be dropped */
	num = 1;

	for (i = client->tail; i != client->head; i = (i + 1) & mask) {
		ev = &client->buffer[i];
		is_report = ev->type == EV_SYN && ev->code == SYN_REPORT;

		if (ev->type == type) {
			/* drop matched entry */
			continue;
		} else if (is_report && !num) {
			/* drop empty SYN_REPORT groups */
			continue;
		} else if (head != i) {
			/* move entry to fill the gap */
			client->buffer[head].time = ev->time;
			client->buffer[head].type = ev->type;
			client->buffer[head].code = ev->code;
			client->buffer[head].value = ev->value;
		}

		num++;
		head = (head + 1) & mask;

		if (is_report) {
			num = 0;
			client->packet_head = head;
		}
	}

	client->head = head;
}