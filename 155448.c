static int __init amba_init(void)
{
	return bus_register(&amba_bustype);
}