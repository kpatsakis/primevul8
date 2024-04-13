static int kvm_cpu_hotplug(struct notifier_block *notifier, unsigned long val,
			   void *v)
{
	int cpu = (long)v;

	if (!kvm_usage_count)
		return NOTIFY_OK;

	val &= ~CPU_TASKS_FROZEN;
	switch (val) {
	case CPU_DYING:
		printk(KERN_INFO "kvm: disabling virtualization on CPU%d\n",
		       cpu);
		hardware_disable(NULL);
		break;
	case CPU_STARTING:
		printk(KERN_INFO "kvm: enabling virtualization on CPU%d\n",
		       cpu);
		hardware_enable(NULL);
		break;
	}
	return NOTIFY_OK;
}