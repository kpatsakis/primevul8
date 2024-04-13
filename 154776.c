long long VideoTrack::GetDisplayHeight() const {
  return m_display_height > 0 ? m_display_height : GetHeight();
}