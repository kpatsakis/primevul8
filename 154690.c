Type* SafeArrayAlloc(unsigned long long num_elements,
                     unsigned long long element_size) {
  if (num_elements == 0 || element_size == 0)
    return NULL;

  const size_t kMaxAllocSize = 0x80000000;  // 2GiB
  const unsigned long long num_bytes = num_elements * element_size;
  if (element_size > (kMaxAllocSize / num_elements))
    return NULL;
  if (num_bytes != static_cast<size_t>(num_bytes))
    return NULL;

  return new (std::nothrow) Type[static_cast<size_t>(num_bytes)];
}