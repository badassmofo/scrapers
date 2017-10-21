#!/usr/bin/env ruby

require 'rmagick'

if __FILE__ == $0
  screen_size  = `system_profiler SPDisplaysDataType | grep -Eohm1 "[0-9]{4} x [0-9]{4}" | tr -d [:space:]`
  imagick_args = $*[1..-1].join(" ")

  img = Magick::Image.read($*[0]).first
  x   = img.columns
  y   = img.rows
  ret = {}

  add_to_ret = ->(_x, _y) {
    px   = img.pixel_color _x, _y
    px_f = "#{px.red / 257},#{px.green / 257},#{px.blue / 257}"
    (ret.has_key? px_f) ? ret[px_f] += 1 : ret[px_f] = 1
  }

  x.times do |i|
    add_to_ret[i, 0]
    add_to_ret[i, y]
  end
  y.times do |i|
    add_to_ret[0, i]
    add_to_ret[x, i]
  end

  `convert -size #{screen_size} xc:'rgba(#{ret.keys.sort { |_x,_y| ret[_x] <=> ret[_y] }[-1]},1)' #{$*[0]} -compose atop #{imagick_args} -composite  output.png`
end
