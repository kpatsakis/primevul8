static struct hso_serial *get_serial_by_index(unsigned index)
{
	struct hso_serial *serial = NULL;
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	if (serial_table[index])
		serial = dev2ser(serial_table[index]);
	spin_unlock_irqrestore(&serial_table_lock, flags);

	return serial;
}