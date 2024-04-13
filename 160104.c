int LibRaw::have_fpdata()
{
  return imgdata.rawdata.float_image || imgdata.rawdata.float3_image ||
         imgdata.rawdata.float4_image;
}