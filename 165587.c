static void start_sw_tscdeadline(struct kvm_lapic *apic)
{
	struct kvm_timer *ktimer = &apic->lapic_timer;
	u64 guest_tsc, tscdeadline = ktimer->tscdeadline;
	u64 ns = 0;
	ktime_t expire;
	struct kvm_vcpu *vcpu = apic->vcpu;
	unsigned long this_tsc_khz = vcpu->arch.virtual_tsc_khz;
	unsigned long flags;
	ktime_t now;

	if (unlikely(!tscdeadline || !this_tsc_khz))
		return;

	local_irq_save(flags);

	now = ktime_get();
	guest_tsc = kvm_read_l1_tsc(vcpu, rdtsc());

	ns = (tscdeadline - guest_tsc) * 1000000ULL;
	do_div(ns, this_tsc_khz);

	if (likely(tscdeadline > guest_tsc) &&
	    likely(ns > apic->lapic_timer.timer_advance_ns)) {
		expire = ktime_add_ns(now, ns);
		expire = ktime_sub_ns(expire, ktimer->timer_advance_ns);
		hrtimer_start(&ktimer->timer, expire, HRTIMER_MODE_ABS_HARD);
	} else
		apic_timer_expired(apic, false);

	local_irq_restore(flags);
}