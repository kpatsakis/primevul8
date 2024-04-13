writeRandomBytes_rand_s(void *target, size_t count) {
  size_t bytesWrittenTotal = 0;

  while (bytesWrittenTotal < count) {
    unsigned int random32 = 0;
    size_t i = 0;

    if (rand_s(&random32))
      return 0; /* failure */

    for (; (i < sizeof(random32)) && (bytesWrittenTotal < count);
         i++, bytesWrittenTotal++) {
      const uint8_t random8 = (uint8_t)(random32 >> (i * 8));
      ((uint8_t *)target)[bytesWrittenTotal] = random8;
    }
  }
  return 1; /* success */
}