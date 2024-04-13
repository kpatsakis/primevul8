static inline struct kvm *mmu_notifier_to_kvm(struct mmu_notifier *mn)
{
	return container_of(mn, struct kvm, mmu_notifier);
}