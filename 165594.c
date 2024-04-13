static inline void apic_set_spiv(struct kvm_lapic *apic, u32 val)
{
	bool enabled = val & APIC_SPIV_APIC_ENABLED;

	kvm_lapic_set_reg(apic, APIC_SPIV, val);

	if (enabled != apic->sw_enabled) {
		apic->sw_enabled = enabled;
		if (enabled)
			static_branch_slow_dec_deferred(&apic_sw_disabled);
		else
			static_branch_inc(&apic_sw_disabled.key);

		atomic_set_release(&apic->vcpu->kvm->arch.apic_map_dirty, DIRTY);
	}

	/* Check if there are APF page ready requests pending */
	if (enabled)
		kvm_make_request(KVM_REQ_APF_READY, apic->vcpu);
}