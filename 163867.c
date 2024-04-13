int read_tag(uint8_t* arr, int pos,  int swapBytes, void* dest)
{
        // Format should be 5 over here (rational)
    uint32_t format = read16(arr, pos + 2, swapBytes);
    // Components should be 1
    uint32_t components = read32(arr, pos + 4, swapBytes);
    // Points to the value
    uint32_t offset;

    // sanity
    if (components != 1) return 0;

    if (format == 3)
        offset = pos + 8;
    else
        offset =  read32(arr, pos + 8, swapBytes);

    switch (format) {

    case 5: // Rational
          {
          double num = read32(arr, offset, swapBytes);
          double den = read32(arr, offset + 4, swapBytes);
          *(double *) dest = num / den;
          }
          break;

    case 3: // uint 16
        *(int*) dest = read16(arr, offset, swapBytes);
        break;

    default:  return 0;
    }

    return 1;
}