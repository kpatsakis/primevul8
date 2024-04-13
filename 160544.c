int LibRaw::cameraCount()
{
  return (sizeof(static_camera_list) / sizeof(static_camera_list[0])) - 1;
}