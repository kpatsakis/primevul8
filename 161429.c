void __init hrtimers_init(void)
{
	hrtimers_prepare_cpu(smp_processor_id());
}