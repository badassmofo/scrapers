#!/usr/bin/env ruby
require 'json'
require 'fastimage'

class Array
  def median
    sorted = self.sort
    mid = (sorted.length - 1) / 2.0
    (sorted[mid.floor] + sorted[mid.ceil]) / 2.0
  end
end

class Fixnum
  def divisors
    (1..self).select { |x| self % x == 0 }
  end
end

exts   = [".png", ".bmp", ".jpg", ".jpeg"]
imgs   = ARGV.select { |a| (File.exists?(a) && exts.include?(File.extname(a))) }
row_l  = imgs.length.divisors.median.ceil
out    = []
imgs.each_with_index do |f, i|
  w, h = FastImage.size f
  c, r = i / row_l, i % row_l
  out.push [] if c == 0
  x = (r == 0 ? 0 : out[c][r - 1][:x] + out[c][r - 1][:width] + 1)
  y = (c == 0 ? 0 : out[c - 1][r][:y] + out[c - 1][r][:height] + 1)
  out[c].push({ :src => f, :width => w, :height => h, :x => x, :y => y })
end
out = out.flatten
puts `./spritesheet_gen spritesheet.png #{out.length} '#{out.to_json}'`
exit $?.exitstatus
