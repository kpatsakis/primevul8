static int handle_task_switch(struct kvm_vcpu *vcpu)
{
	struct vcpu_vmx *vmx = to_vmx(vcpu);
	unsigned long exit_qualification;
	bool has_error_code = false;
	u32 error_code = 0;
	u16 tss_selector;
	int reason, type, idt_v, idt_index;

	idt_v = (vmx->idt_vectoring_info & VECTORING_INFO_VALID_MASK);
	idt_index = (vmx->idt_vectoring_info & VECTORING_INFO_VECTOR_MASK);
	type = (vmx->idt_vectoring_info & VECTORING_INFO_TYPE_MASK);

	exit_qualification = vmx_get_exit_qual(vcpu);

	reason = (u32)exit_qualification >> 30;
	if (reason == TASK_SWITCH_GATE && idt_v) {
		switch (type) {
		case INTR_TYPE_NMI_INTR:
			vcpu->arch.nmi_injected = false;
			vmx_set_nmi_mask(vcpu, true);
			break;
		case INTR_TYPE_EXT_INTR:
		case INTR_TYPE_SOFT_INTR:
			kvm_clear_interrupt_queue(vcpu);
			break;
		case INTR_TYPE_HARD_EXCEPTION:
			if (vmx->idt_vectoring_info &
			    VECTORING_INFO_DELIVER_CODE_MASK) {
				has_error_code = true;
				error_code =
					vmcs_read32(IDT_VECTORING_ERROR_CODE);
			}
			fallthrough;
		case INTR_TYPE_SOFT_EXCEPTION:
			kvm_clear_exception_queue(vcpu);
			break;
		default:
			break;
		}
	}
	tss_selector = exit_qualification;

	if (!idt_v || (type != INTR_TYPE_HARD_EXCEPTION &&
		       type != INTR_TYPE_EXT_INTR &&
		       type != INTR_TYPE_NMI_INTR))
		WARN_ON(!skip_emulated_instruction(vcpu));

	/*
	 * TODO: What about debug traps on tss switch?
	 *       Are we supposed to inject them and update dr6?
	 */
	return kvm_task_switch(vcpu, tss_selector,
			       type == INTR_TYPE_SOFT_INTR ? idt_index : -1,
			       reason, has_error_code, error_code);
}