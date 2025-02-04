int handle_ud(struct kvm_vcpu *vcpu)
{
	int emul_type = EMULTYPE_TRAP_UD;
	enum emulation_result er;
	char sig[5]; /* ud2; .ascii "kvm" */
	struct x86_exception e;

	if (force_emulation_prefix &&
	    kvm_read_guest_virt(vcpu, kvm_get_linear_rip(vcpu),
				sig, sizeof(sig), &e) == 0 &&
	    memcmp(sig, "\xf\xbkvm", sizeof(sig)) == 0) {
		kvm_rip_write(vcpu, kvm_rip_read(vcpu) + sizeof(sig));
		emul_type = 0;
	}

	er = kvm_emulate_instruction(vcpu, emul_type);
	if (er == EMULATE_USER_EXIT)
		return 0;
	if (er != EMULATE_DONE)
		kvm_queue_exception(vcpu, UD_VECTOR);
	return 1;
}