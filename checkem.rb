#!/usr/bin/env ruby
require 'net/http'
require 'json'

abort "No arguments passed!" if $*.empty?
$*.each do |x|
  if x =~ /^https?:\/\/boards.4chan.org\/([a-eg-ikm-z3]|gif|vg|vr|wg|ic|r9k|s4s|cm|hm|lgbt|adv|an|asp|biz|cgl|ck|co|diy|fa|fit|gd|hc|his|int|jp|lit|mlp|mu|out|po|pol|sci|soc|sp|tg|toy|trv|tv|vp|wsg|wsr|rs)\/thread\/(\d+)$/i
    board, thread = $1, $2
    JSON.parse((Net::HTTP.get_response 'api.4chan.org', "/#{board}/res/#{thread}.json").body)['posts'].each do |y|
      puts ">>#{y['no']}\nnice #{['dubs', 'trips', 'quads', 'quints', 'sexts', 'septs', 'octs'][$2.length - 1]}"  if y['no'].to_s =~ /(\d)(\1+)$/
    end
  else
    puts "ERROR! Invalid board or ID (#{x})"
  end
end
