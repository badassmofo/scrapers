#!/usr/bin/env ruby

if __FILE__ == $0
  dirs = $*.uniq.select { |x| File.directory? x }
  dirs = [ Dir.pwd ] if dirs.empty?

  dirs.each_with_index do |x, i|
    Dir.entries(x).each do |y|
      y = "#{x}#{'/' unless x[-1] == '/'}#{y}"
      f = File.basename y, '.*'
      f.slice! $& if f =~ /([A-Z0-9-]+)\.([A-Z0-9]{2,6})/i
      if f =~ /([A-Z]{2,6})-?(\d+)/i
        n = "#{File.dirname y}/#{$1.upcase}-#{$2}#{File.extname y}"
        puts "#{File.basename y} => #{File.basename n}" if File.rename y, n
      end
    end
  end
end

