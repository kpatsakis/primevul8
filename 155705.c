writeRandomBytes_arc4random(void *target, size_t count) {
  size_t bytesWrittenTotal = 0;

  while (bytesWrittenTotal < count) {
    const uint32_t random32 = arc4random();
    size_t i = 0;

    for (; (i < sizeof(random32)) && (bytesWrittenTotal < count);
         i++, bytesWrittenTotal++) {
      const uint8_t random8 = (uint8_t)(random32 >> (i * 8));
      ((uint8_t *)target)[bytesWrittenTotal] = random8;
    }
  }
}