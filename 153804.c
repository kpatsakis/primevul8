static int stbi__jpeg_decode_block_prog_ac(stbi__jpeg *j, short data[64], stbi__huffman *hac, stbi__int16 *fac)
{
   int k;
   if (j->spec_start == 0) return stbi__err("can't merge dc and ac", "Corrupt JPEG");

   if (j->succ_high == 0) {
      int shift = j->succ_low;

      if (j->eob_run) {
         --j->eob_run;
         return 1;
      }

      k = j->spec_start;
      do {
         unsigned int zig;
         int c,r,s;
         if (j->code_bits < 16) stbi__grow_buffer_unsafe(j);
         c = (j->code_buffer >> (32 - FAST_BITS)) & ((1 << FAST_BITS)-1);
         r = fac[c];
         if (r) { // fast-AC path
            k += (r >> 4) & 15; // run
            s = r & 15; // combined length
            j->code_buffer <<= s;
            j->code_bits -= s;
            zig = stbi__jpeg_dezigzag[k++];
            data[zig] = (short) ((r >> 8) * (1 << shift));
         } else {
            int rs = stbi__jpeg_huff_decode(j, hac);
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r);
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  --j->eob_run;
                  break;
               }
               k += 16;
            } else {
               k += r;
               zig = stbi__jpeg_dezigzag[k++];
               data[zig] = (short) (stbi__extend_receive(j,s) * (1 << shift));
            }
         }
      } while (k <= j->spec_end);
   } else {
      // refinement scan for these AC coefficients

      short bit = (short) (1 << j->succ_low);

      if (j->eob_run) {
         --j->eob_run;
         for (k = j->spec_start; k <= j->spec_end; ++k) {
            short *p = &data[stbi__jpeg_dezigzag[k]];
            if (*p != 0)
               if (stbi__jpeg_get_bit(j))
                  if ((*p & bit)==0) {
                     if (*p > 0)
                        *p += bit;
                     else
                        *p -= bit;
                  }
         }
      } else {
         k = j->spec_start;
         do {
            int r,s;
            int rs = stbi__jpeg_huff_decode(j, hac); // @OPTIMIZE see if we can use the fast path here, advance-by-r is so slow, eh
            if (rs < 0) return stbi__err("bad huffman code","Corrupt JPEG");
            s = rs & 15;
            r = rs >> 4;
            if (s == 0) {
               if (r < 15) {
                  j->eob_run = (1 << r) - 1;
                  if (r)
                     j->eob_run += stbi__jpeg_get_bits(j, r);
                  r = 64; // force end of block
               } else {
                  // r=15 s=0 should write 16 0s, so we just do
                  // a run of 15 0s and then write s (which is 0),
                  // so we don't have to do anything special here
               }
            } else {
               if (s != 1) return stbi__err("bad huffman code", "Corrupt JPEG");
               // sign bit
               if (stbi__jpeg_get_bit(j))
                  s = bit;
               else
                  s = -bit;
            }

            // advance by r
            while (k <= j->spec_end) {
               short *p = &data[stbi__jpeg_dezigzag[k++]];
               if (*p != 0) {
                  if (stbi__jpeg_get_bit(j))
                     if ((*p & bit)==0) {
                        if (*p > 0)
                           *p += bit;
                        else
                           *p -= bit;
                     }
               } else {
                  if (r == 0) {
                     *p = (short) s;
                     break;
                  }
                  --r;
               }
            }
         } while (k <= j->spec_end);
      }
   }
   return 1;
}