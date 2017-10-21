#!/usr/bin/env ruby
require 'fileutils'

$*.delete_if { |x| not File.directory? x }.each do |x|
  x += '/' unless x[-1] == '/'
  d = Dir.glob(x + "**/*/").sort_by { |s| s.count '/' }.reverse
  d.each do |y|
    Dir.glob(y + '*').delete_if { |a| File.directory? a }.each do |z|
      to = "#{x}#{z.split('/')[-1]}"
      if File.exists? to
        n = 2
        to_parts = to.split('.')
        ext      = to_parts[-1]
        f_name   = to_parts[0..-2].join '.'
        while true do
          test = "#{f_name} (#{n}).#{ext}"
          unless File.exists? test
            to = test
            break
          end
          n += 1
        end
      end
      File.rename z, to
      puts "#{z} => #{to}"
    end
  end
  d.each do |b|
    puts "Removing directroy \"#{b}\""
    FileUtils.rm_r b
  end
end
