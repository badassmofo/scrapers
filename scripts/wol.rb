#!/usr/bin/env ruby
require 'socket'

config = {
  'host'  => '255.255.255',
  'local' => true,
  'port'  => 9 }

abort "./#{$0} [MAC address (required)]\n\t [Host (default: 255.255.255)]\n\t [Local (default: true)]\n\t [port (default: 9)]" unless not $*.empty? or $*[0] =~ /^([A-F0-9]{2}[:]){5}([A-F0-9]{2})$/i

if __FILE__ == $0
  config['host']  = $*[1] if $*[1] =~ /^(([0-9]{1,3}\.){3}([0-9]{1,3})|(https?:\/\/)?(www\.)?([a-z0-9-]+)\.([a-z0-9\.]{2,6}))$/
  config['local'] = $*[2].downcase == 'true' if $*[2] =~ /^(true|false)$/i
  config['port']  = $*[3].to_i if $*[3] =~ /^(\d+)$/

  magic = 0xFF.chr * 6 + [$*[0].gsub(/:/, '')].pack('H12') * 16
  bytes = UDPSocket.open do |s|
    s.setsockopt Socket::SOL_SOCKET, Socket::SO_BROADCAST, 1 if config['local']
    s.send magic, 0, config['host'], config['port']
  end
  puts "#{bytes} bytes sent to #{config['host']}:#{config['port']}"
end
