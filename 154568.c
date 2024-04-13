vmx_adjust_secondary_exec_control(struct vcpu_vmx *vmx, u32 *exec_control,
				  u32 control, bool enabled, bool exiting)
{
	/*
	 * If the control is for an opt-in feature, clear the control if the
	 * feature is not exposed to the guest, i.e. not enabled.  If the
	 * control is opt-out, i.e. an exiting control, clear the control if
	 * the feature _is_ exposed to the guest, i.e. exiting/interception is
	 * disabled for the associated instruction.  Note, the caller is
	 * responsible presetting exec_control to set all supported bits.
	 */
	if (enabled == exiting)
		*exec_control &= ~control;

	/*
	 * Update the nested MSR settings so that a nested VMM can/can't set
	 * controls for features that are/aren't exposed to the guest.
	 */
	if (nested) {
		if (enabled)
			vmx->nested.msrs.secondary_ctls_high |= control;
		else
			vmx->nested.msrs.secondary_ctls_high &= ~control;
	}
}