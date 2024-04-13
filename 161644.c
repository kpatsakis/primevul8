static int __init setup_hrtimer_hres(char *str)
{
	return (kstrtobool(str, &hrtimer_hres_enabled) == 0);
}