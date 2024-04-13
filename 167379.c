static int get_free_serial_index(void)
{
	int index;
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	for (index = 0; index < HSO_SERIAL_TTY_MINORS; index++) {
		if (serial_table[index] == NULL) {
			spin_unlock_irqrestore(&serial_table_lock, flags);
			return index;
		}
	}
	spin_unlock_irqrestore(&serial_table_lock, flags);

	pr_err("%s: no free serial devices in table\n", __func__);
	return -1;
}