ENTROPY_DEBUG(const char *label, unsigned long entropy) {
  if (getDebugLevel("EXPAT_ENTROPY_DEBUG", 0) >= 1u) {
    fprintf(stderr, "expat: Entropy: %s --> 0x%0*lx (%lu bytes)\n", label,
            (int)sizeof(entropy) * 2, entropy, (unsigned long)sizeof(entropy));
  }
  return entropy;
}