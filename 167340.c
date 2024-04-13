static struct hso_serial *get_serial_by_shared_int_and_type(
					struct hso_shared_int *shared_int,
					int mux)
{
	int i, port;

	port = hso_mux_to_port(mux);

	for (i = 0; i < HSO_SERIAL_TTY_MINORS; i++) {
		if (serial_table[i] &&
		    (dev2ser(serial_table[i])->shared_int == shared_int) &&
		    ((serial_table[i]->port_spec & HSO_PORT_MASK) == port)) {
			return dev2ser(serial_table[i]);
		}
	}

	return NULL;
}