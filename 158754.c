static int __init serial_ir_init_module(void)
{
	switch (type) {
	case IR_HOMEBREW:
	case IR_IRDEO:
	case IR_IRDEO_REMOTE:
	case IR_ANIMAX:
	case IR_IGOR:
		/* if nothing specified, use ttyS0/com1 and irq 4 */
		io = io ? io : 0x3f8;
		irq = irq ? irq : 4;
		break;
	default:
		return -EINVAL;
	}
	if (!softcarrier) {
		switch (type) {
		case IR_HOMEBREW:
		case IR_IGOR:
			hardware[type].set_send_carrier = false;
			hardware[type].set_duty_cycle = false;
			break;
		}
	}

	/* make sure sense is either -1, 0, or 1 */
	if (sense != -1)
		sense = !!sense;

	return serial_ir_init();
}