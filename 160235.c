static inline void unpack7bytesto4x16_nikon(unsigned char *src,
                                            unsigned short *dest)
{
  dest[3] = (src[6] << 6) | (src[5] >> 2);
  dest[2] = ((src[5] & 0x3) << 12) | (src[4] << 4) | (src[3] >> 4);
  dest[1] = (src[3] & 0xf) << 10 | (src[2] << 2) | (src[1] >> 6);
  dest[0] = ((src[1] & 0x3f) << 8) | src[0];
}