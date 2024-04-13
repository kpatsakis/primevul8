static inline void unpack28bytesto16x16ns(unsigned char *src,
                                          unsigned short *dest)
{
  dest[0] = (src[3] << 6) | (src[2] >> 2);
  dest[1] = ((src[2] & 0x3) << 12) | (src[1] << 4) | (src[0] >> 4);
  dest[2] = (src[0] & 0xf) << 10 | (src[7] << 2) | (src[6] >> 6);
  dest[3] = ((src[6] & 0x3f) << 8) | src[5];
  dest[4] = (src[4] << 6) | (src[11] >> 2);
  dest[5] = ((src[11] & 0x3) << 12) | (src[10] << 4) | (src[9] >> 4);
  dest[6] = (src[9] & 0xf) << 10 | (src[8] << 2) | (src[15] >> 6);
  dest[7] = ((src[15] & 0x3f) << 8) | src[14];
  dest[8] = (src[13] << 6) | (src[12] >> 2);
  dest[9] = ((src[12] & 0x3) << 12) | (src[19] << 4) | (src[18] >> 4);
  dest[10] = (src[18] & 0xf) << 10 | (src[17] << 2) | (src[16] >> 6);
  dest[11] = ((src[16] & 0x3f) << 8) | src[23];
  dest[12] = (src[22] << 6) | (src[21] >> 2);
  dest[13] = ((src[21] & 0x3) << 12) | (src[20] << 4) | (src[27] >> 4);
  dest[14] = (src[27] & 0xf) << 10 | (src[26] << 2) | (src[25] >> 6);
  dest[15] = ((src[25] & 0x3f) << 8) | src[24];
}