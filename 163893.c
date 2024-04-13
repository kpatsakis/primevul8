void jcopy_markers_execute(j_decompress_ptr srcinfo, j_compress_ptr dstinfo)
{
  jpeg_saved_marker_ptr marker;

  /* In the current implementation, we don't actually need to examine the
   * option flag here; we just copy everything that got saved.
   * But to avoid confusion, we do not output JFIF and Adobe APP14 markers
   * if the encoder library already wrote one.
   */
  for (marker = srcinfo->marker_list; marker != NULL; marker = marker->next) {

    if (dstinfo->write_JFIF_header &&
        marker->marker == JPEG_APP0 &&
        marker->data_length >= 5 &&
        GETJOCTET(marker->data[0]) == 0x4A &&
        GETJOCTET(marker->data[1]) == 0x46 &&
        GETJOCTET(marker->data[2]) == 0x49 &&
        GETJOCTET(marker->data[3]) == 0x46 &&
        GETJOCTET(marker->data[4]) == 0)
                          continue;         /* reject duplicate JFIF */

    if (dstinfo->write_Adobe_marker &&
        marker->marker == JPEG_APP0+14 &&
        marker->data_length >= 5 &&
        GETJOCTET(marker->data[0]) == 0x41 &&
        GETJOCTET(marker->data[1]) == 0x64 &&
        GETJOCTET(marker->data[2]) == 0x6F &&
        GETJOCTET(marker->data[3]) == 0x62 &&
        GETJOCTET(marker->data[4]) == 0x65)
                         continue;         /* reject duplicate Adobe */

     jpeg_write_marker(dstinfo, marker->marker,
                       marker->data, marker->data_length);
  }
}