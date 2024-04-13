uint64 MakeUID(unsigned int* seed) {
  uint64 uid = 0;

#ifdef __MINGW32__
  srand(*seed);
#endif

  for (int i = 0; i < 7; ++i) {  // avoid problems with 8-byte values
    uid <<= 8;

// TODO(fgalligan): Move random number generation to platform specific code.
#ifdef _MSC_VER
    (void)seed;
    const int32 nn = rand();
#elif __ANDROID__
    (void)seed;
    int32 temp_num = 1;
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd != -1) {
      read(fd, &temp_num, sizeof(temp_num));
      close(fd);
    }
    const int32 nn = temp_num;
#elif defined __MINGW32__
    const int32 nn = rand();
#else
    const int32 nn = rand_r(seed);
#endif
    const int32 n = 0xFF & (nn >> 4);  // throw away low-order bits

    uid |= n;
  }

  return uid;
}