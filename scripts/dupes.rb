#!/usr/bin/ruby
require 'digest/md5'

$*.select { |f| File.file? f }.inject({}) { |k, v| k[v] = Digest::MD5.file(v).hexdigest; k}.group_by {|k,v| v}.select {|k, v| v.length > 1}.each do |k, v|
  Hash[*v.flatten].keys.sort_by(&File.method(:mtime))[1..-1].each do |x|
    y = "#{Dir.home}/.dupes/#{x.split('/')[-1]}"
    puts "Moving #{x} => #{y}"
    File.rename x, y
  end
end
