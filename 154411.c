static inline bool vmx_feature_control_msr_valid(struct kvm_vcpu *vcpu,
						 uint64_t val)
{
	uint64_t valid_bits = to_vmx(vcpu)->msr_ia32_feature_control_valid_bits;

	return !(val & ~valid_bits);
}