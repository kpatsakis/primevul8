static void update_target_expiration(struct kvm_lapic *apic, uint32_t old_divisor)
{
	ktime_t now, remaining;
	u64 ns_remaining_old, ns_remaining_new;

	apic->lapic_timer.period =
			tmict_to_ns(apic, kvm_lapic_get_reg(apic, APIC_TMICT));
	limit_periodic_timer_frequency(apic);

	now = ktime_get();
	remaining = ktime_sub(apic->lapic_timer.target_expiration, now);
	if (ktime_to_ns(remaining) < 0)
		remaining = 0;

	ns_remaining_old = ktime_to_ns(remaining);
	ns_remaining_new = mul_u64_u32_div(ns_remaining_old,
	                                   apic->divide_count, old_divisor);

	apic->lapic_timer.tscdeadline +=
		nsec_to_cycles(apic->vcpu, ns_remaining_new) -
		nsec_to_cycles(apic->vcpu, ns_remaining_old);
	apic->lapic_timer.target_expiration = ktime_add_ns(now, ns_remaining_new);
}