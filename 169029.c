void vt_clr_kbd_mode_bit(int console, int bit)
{
	struct kbd_struct *kb = kbd_table + console;
	unsigned long flags;

	spin_lock_irqsave(&kbd_event_lock, flags);
	clr_vc_kbd_mode(kb, bit);
	spin_unlock_irqrestore(&kbd_event_lock, flags);
}