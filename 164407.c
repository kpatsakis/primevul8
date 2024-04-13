static int handle_eviocgbit(struct input_dev *dev,
			    unsigned int type, unsigned int size,
			    void __user *p, int compat_mode)
{
	unsigned long *bits;
	int len;

	switch (type) {

	case      0: bits = dev->evbit;  len = EV_MAX;  break;
	case EV_KEY: bits = dev->keybit; len = KEY_MAX; break;
	case EV_REL: bits = dev->relbit; len = REL_MAX; break;
	case EV_ABS: bits = dev->absbit; len = ABS_MAX; break;
	case EV_MSC: bits = dev->mscbit; len = MSC_MAX; break;
	case EV_LED: bits = dev->ledbit; len = LED_MAX; break;
	case EV_SND: bits = dev->sndbit; len = SND_MAX; break;
	case EV_FF:  bits = dev->ffbit;  len = FF_MAX;  break;
	case EV_SW:  bits = dev->swbit;  len = SW_MAX;  break;
	default: return -EINVAL;
	}

	return bits_to_user(bits, len, size, p, compat_mode);
}