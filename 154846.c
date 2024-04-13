long long VideoTrack::GetDisplayWidth() const {
  return m_display_width > 0 ? m_display_width : GetWidth();
}