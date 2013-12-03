require 'webrick'
require 'ffi'

module Keys
  VK_VOLUME_DOWN = 0xAE
  VK_VOLUME_UP = 0xAF
  VK_MEDIA_PLAY_PAUSE = 0xB3
  VK_MEDIA_NEXT_TRACK = 0xB0
  KEYEVENTF_KEYUP = 2

  extend FFI::Library
  ffi_lib 'user32'
  ffi_convention :stdcall

  attach_function :keybd_event, [ :uchar, :uchar, :int, :pointer ], :void

  def self.press_key(key)
    keybd_event(key, 0, 0, nil)
    keybd_event(key, 0, KEYEVENTF_KEYUP, nil)
  end

  def self.play_pause
    self.press_key VK_MEDIA_PLAY_PAUSE
  end

  def self.next
    press_key VK_MEDIA_NEXT_TRACK
  end

  def self.vol_up
    press_key VK_VOLUME_UP
  end

  def self.vol_down
    press_key VK_VOLUME_DOWN
  end
end

server = WEBrick::HTTPServer.new :Port => 8000

server.mount_proc '/' do |req, res|
  res.body = 'Whoo!'
end

server.mount_proc '/pause' do |req, res|
  Keys.play_pause
end

server.mount_proc '/next' do |req, res|
  Keys.next
end

server.mount_proc '/volup' do |req, res|
  Keys.vol_up
end

server.mount_proc '/voldown' do |req, res|
  Keys.vol_down
end

trap 'INT' do server.shutdown end

server.start
