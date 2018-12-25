#!/usr/bin/env ruby
require 'net/http'
require 'json'

if __FILE__ == $0
  open = case RbConfig::CONFIG['host_os']
         when /mswin|mingw|cygwin/
           "start"
         when /darwin/
           "open"
         else
           "xdg-open"
         end

  res = Net::HTTP.get_response('api.4chan.org', '/g/catalog.json')
  abort "ERROR: #{res.code}" unless res.code == '200'

  JSON.parse(res.body).each do |page|
    page['threads'].each do|thread|
      system "#{open} http://boards.4chan.org/g/res/#{thread['no']}" unless['name', 'sub'].select { |field| thread[field] =~ /(d|n)pt|ly programming thread/i }.empty?
    end
  end
end
