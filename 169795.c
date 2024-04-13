
static void intel_hda_exit(PCIDevice *pci)
{
    IntelHDAState *d = INTEL_HDA(pci);

    msi_uninit(&d->pci);