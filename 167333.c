static void set_serial_by_index(unsigned index, struct hso_serial *serial)
{
	unsigned long flags;

	spin_lock_irqsave(&serial_table_lock, flags);
	if (serial)
		serial_table[index] = serial->parent;
	else
		serial_table[index] = NULL;
	spin_unlock_irqrestore(&serial_table_lock, flags);
}