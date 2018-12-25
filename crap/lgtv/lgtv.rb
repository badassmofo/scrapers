#!/usr/bin/env ruby
require 'json'
require 'websocket-client-simple'
require 'thread'

$args = ARGV.inject([]) { |a,element| a << element.dup }
$args = ["interactive"] unless $args.any?
$stdout.sync = true

class Subprocess < Thread
	attr_accessor :open
	attr_accessor :queue

	def initialize
		@pipe = nil
		@open = true
		@queue = Queue.new

		super do
			case RbConfig::CONFIG['host_os']
			when /mswin|msys|mingw|cygwin|bccwin|wince|emc/
				raise Error, "Unsupported OS: #{host_os.inspect}"
			when /darwin|mac os/
				@pipe = IO.popen("sudo lgtv_osx_mk", "r+")
			when /linux|solaris|bsd/
				raise Error, "Unsupported OS: #{host_os.inspect}"
			else
				raise Error, "Unknown OS: #{host_os.inspect}"
			end

			while line = @pipe.gets
				@queue << line
			end
			@open = false
		end
	end

	def stop
		Process.kill "TERM", @pipe.pid
	end
end
$sp = nil

class Emitter
	def callbacks
		@callbacks ||= Hash.new { |h, k| h[k] = [] }
	end

	def on type, &block
		callbacks[type] << block
		self
	end

	def emit type, *args
		callbacks[type].each do |block|
			block.call(*args)
		end
	end
end

hello_new = "{\"type\":\"register\",\"id\":\"register_0\",\"payload\":{\"forcePairing\":false,\"pairingType\":\"PROMPT\",\"manifest\":{\"manifestVersion\":1,\"appVersion\":\"1.1\",\"signed\":{\"created\":\"20140509\",\"appId\":\"com.lge.test\",\"vendorId\":\"com.lge\",\"localizedAppNames\":{\"\":\"LG Remote App\",\"ko-KR\":\"리모컨 앱\",\"zxx-XX\":\"ЛГ Rэмotэ AПП\"},\"localizedVendorNames\":{\"\":\"LG Electronics\"},\"permissions\":[\"TEST_SECURE\",\"CONTROL_INPUT_TEXT\",\"CONTROL_MOUSE_AND_KEYBOARD\",\"READ_INSTALLED_APPS\",\"READ_LGE_SDX\",\"READ_NOTIFICATIONS\",\"SEARCH\",\"WRITE_SETTINGS\",\"WRITE_NOTIFICATION_ALERT\",\"CONTROL_POWER\",\"READ_CURRENT_CHANNEL\",\"READ_RUNNING_APPS\",\"READ_UPDATE_INFO\",\"UPDATE_FROM_REMOTE_APP\",\"READ_LGE_TV_INPUT_EVENTS\",\"READ_TV_CURRENT_TIME\"],\"serial\":\"2f930e2d2cfe083771f68e4fe7bb07\"},\"permissions\":[\"LAUNCH\",\"LAUNCH_WEBAPP\",\"APP_TO_APP\",\"CLOSE\",\"TEST_OPEN\",\"TEST_PROTECTED\",\"CONTROL_AUDIO\",\"CONTROL_DISPLAY\",\"CONTROL_INPUT_JOYSTICK\",\"CONTROL_INPUT_MEDIA_RECORDING\",\"CONTROL_INPUT_MEDIA_PLAYBACK\",\"CONTROL_INPUT_TV\",\"CONTROL_POWER\",\"READ_APP_STATUS\",\"READ_CURRENT_CHANNEL\",\"READ_INPUT_DEVICE_LIST\",\"READ_NETWORK_STATE\",\"READ_RUNNING_APPS\",\"READ_TV_CHANNEL_LIST\",\"WRITE_NOTIFICATION_TOAST\",\"READ_POWER_STATE\",\"READ_COUNTRY_INFO\"],\"signatures\":[{\"signatureVersion\":1,\"signature\":\"eyJhbGdvcml0aG0iOiJSU0EtU0hBMjU2Iiwia2V5SWQiOiJ0ZXN0LXNpZ25pbmctY2VydCIsInNpZ25hdHVyZVZlcnNpb24iOjF9.hrVRgjCwXVvE2OOSpDZ58hR+59aFNwYDyjQgKk3auukd7pcegmE2CzPCa0bJ0ZsRAcKkCTJrWo5iDzNhMBWRyaMOv5zWSrthlf7G128qvIlpMT0YNY+n/FaOHE73uLrS/g7swl3/qH/BGFG2Hu4RlL48eb3lLKqTt2xKHdCs6Cd4RMfJPYnzgvI4BNrFUKsjkcu+WD4OO2A27Pq1n50cMchmcaXadJhGrOqH5YmHdOCj5NSHzJYrsW0HPlpuAx/ECMeIZYDh6RMqaFM2DXzdKX9NmmyqzJ3o/0lkk/N97gfVRLW5hA29yeAwaCViZNCP8iC9aO0q9fQojoa7NQnAtw==\"}]}}}";
hello = "{\"type\":\"register\",\"id\":\"register_0\",\"payload\":{\"forcePairing\":false,\"pairingType\":\"PROMPT\",\"client-key\":\"CLIENTKEYGOESHERE\",\"manifest\":{\"manifestVersion\":1,\"appVersion\":\"1.1\",\"signed\":{\"created\":\"20140509\",\"appId\":\"com.lge.test\",\"vendorId\":\"com.lge\",\"localizedAppNames\":{\"\":\"LG Remote App\",\"ko-KR\":\"리모컨 앱\",\"zxx-XX\":\"ЛГ Rэмotэ AПП\"},\"localizedVendorNames\":{\"\":\"LG Electronics\"},\"permissions\":[\"TEST_SECURE\",\"CONTROL_INPUT_TEXT\",\"CONTROL_MOUSE_AND_KEYBOARD\",\"READ_INSTALLED_APPS\",\"READ_LGE_SDX\",\"READ_NOTIFICATIONS\",\"SEARCH\",\"WRITE_SETTINGS\",\"WRITE_NOTIFICATION_ALERT\",\"CONTROL_POWER\",\"READ_CURRENT_CHANNEL\",\"READ_RUNNING_APPS\",\"READ_UPDATE_INFO\",\"UPDATE_FROM_REMOTE_APP\",\"READ_LGE_TV_INPUT_EVENTS\",\"READ_TV_CURRENT_TIME\"],\"serial\":\"2f930e2d2cfe083771f68e4fe7bb07\"},\"permissions\":[\"LAUNCH\",\"LAUNCH_WEBAPP\",\"APP_TO_APP\",\"CLOSE\",\"TEST_OPEN\",\"TEST_PROTECTED\",\"CONTROL_AUDIO\",\"CONTROL_DISPLAY\",\"CONTROL_INPUT_JOYSTICK\",\"CONTROL_INPUT_MEDIA_RECORDING\",\"CONTROL_INPUT_MEDIA_PLAYBACK\",\"CONTROL_INPUT_TV\",\"CONTROL_POWER\",\"READ_APP_STATUS\",\"READ_CURRENT_CHANNEL\",\"READ_INPUT_DEVICE_LIST\",\"READ_NETWORK_STATE\",\"READ_RUNNING_APPS\",\"READ_TV_CHANNEL_LIST\",\"WRITE_NOTIFICATION_TOAST\",\"READ_POWER_STATE\",\"READ_COUNTRY_INFO\"],\"signatures\":[{\"signatureVersion\":1,\"signature\":\"eyJhbGdvcml0aG0iOiJSU0EtU0hBMjU2Iiwia2V5SWQiOiJ0ZXN0LXNpZ25pbmctY2VydCIsInNpZ25hdHVyZVZlcnNpb24iOjF9.hrVRgjCwXVvE2OOSpDZ58hR+59aFNwYDyjQgKk3auukd7pcegmE2CzPCa0bJ0ZsRAcKkCTJrWo5iDzNhMBWRyaMOv5zWSrthlf7G128qvIlpMT0YNY+n/FaOHE73uLrS/g7swl3/qH/BGFG2Hu4RlL48eb3lLKqTt2xKHdCs6Cd4RMfJPYnzgvI4BNrFUKsjkcu+WD4OO2A27Pq1n50cMchmcaXadJhGrOqH5YmHdOCj5NSHzJYrsW0HPlpuAx/ECMeIZYDh6RMqaFM2DXzdKX9NmmyqzJ3o/0lkk/N97gfVRLW5hA29yeAwaCViZNCP8iC9aO0q9fQojoa7NQnAtw==\"}]}}}";

path = File.expand_path "~/.lgtv.json"
config = JSON.parse File.read(path) if File.exists? path

exit "\e[1;31mERROR!\e[0m ~/.lgtv.json doesn't exist or doens't contain and IP." if not config or not config.has_key? "ip"

$debug_msg = (if config.has_key? 'debug' then config['debug'] else false end)
$status_msg = (if config.has_key? 'status' then config['status'] else true end)
exit_code = 0

$emitter = Emitter.new
$ws = WebSocket::Client::Simple.connect "ws://#{config['ip']}:3000"
$pointer_ws = nil

def str_to_bool(str)
	case str.downcase
	when true, 'true', 1, '1', 't', 'yes', 'y', 'on' then true
	when false, 'false', nil, '', 0, '0', 'f', 'no', 'n', 'off' then false
	else
		raise ArgumentError, "invalid value for str_to_bool(): \"#{value.inspect}\""
	end
end

def parse_inputs(data)
	data["payload"]["devices"].each do |d|
		puts "#{d['id']},#{d['label']},#{d['port']}"
	end
end

def parse_volume(data)
	puts "\e[1;37mVolume\e[0m: #{data['payload']['volume']}"
	puts "\e[1;37mMuted\e[0m:  #{data['payload']['muted'].to_s}"
end

def toggle_mute(data) 
	send_command "", "request", "ssap://audio/setMute", "{\"mute\": #{(!data["payload"]["mute"]).to_s}}"
end

def parse_apps(data)
	data["payload"]["launchPoints"].each do |lp|
		puts "\e[1;37mTitle\e[0m:  #{lp['title']}"
		puts "\e[1;37mID\e[0m:     #{lp['id']}"
		puts "\e[1;37mSystem?\e[0m #{lp['systemApp'].to_s}, \e[1;37mRemovable?\e[0m #{lp['removable'].to_s}"
	end
end

def parse_info(data)
	data = data['payload']
	puts "\e[1;37mOS\e[0m:      #{data['product_name']}"
	puts "\e[1;37mModel\e[0m:   #{data['model_name']}"
	puts "\e[1;37mVersion\e[0m: #{data['major_ver']}.#{data['minor_ver']}"
	puts "\e[1;37mCountry\e[0m: #{data['country']}, #{data['language_code']}"
	puts "\e[1;37mDevice\e[0m:  #{data['device_id']}"
end

def parse_services(data)
	data['payload']['services'].each do |s|
		puts "\e[1;37mName\e[0m:    #{s['name']}"
		puts "\e[1;37mVersion\e[0m: #{s['version']}"
	end
end

def parse_channels(data)
	# TODO
end

def setup_pointer_ws(data)
	send_command "", "subscribe", "ssap://com.webos.service.ime/registerRemoteKeyboard", nil

	$pointer_ws = WebSocket::Client::Simple.connect data["payload"]["socketPath"]

	$pointer_ws.on :message do |msg|
		begin
			puts "\e[1;37mDEBUG\e[0m: << #{msg.data}" if $debug_msg
			resp = JSON.parse msg.data
			if resp.has_key? "error"
				$emitter.emit :error, resp['error']
			else
				$emitter.emit resp["id"].to_sym, resp
			end
		rescue JSON::ParserError => e
			$emitter.emit :error, "#{msg.data}, #{e.inspect}"
		end
	end

	$pointer_ws.on :open do
		puts "--- \e[1;37mPOINTER CONNECTION OPENED\e[0m ---" if $status_msg
	end

	$pointer_ws.on :close do |e|
		puts "--- \e[1;37mPOINTER CONNECTION CLOSED\e[0m ---" if $status_msg
	end

	$pointer_ws.on :error do |e|
	end

	# caps_locked = false
	$sp = Subprocess.new
	while $sp.open
		until $sp.queue.empty?
			dx, dy, swdx, swdy, event_mask, key, key_str, mod, mod_str = $sp.queue.pop.split(',')
      # puts "dx: #{dx}, dy: #{dy}, swdx: #{swdx}, swdy: #{swdy} event: #{event_mask}, key: #{key} #{key_str}, mod: #{mod} #{mod_str}"
			mods = if mod_str == "(null)" then [] else mod_str.split(':') end
			# caps_locked = !caps_locked if mods.include?("CAPSLOCK")
			key_str = key_str.upcase if mods.include?("SHIFT")

			event_mask = event_mask.to_i
			case event_mask
			when 32
				$pointer_ws.send "type:move\ndx:#{dx}\ndy:#{dy}\ndown:0\n\n"
			when 2
				$pointer_ws.send "type:click\n\n"
			when 1
				$pointer_ws.send "type:scroll\ndx:#{swdx}\ndy:#{swdy}\n\n"
			when 1024
				case key.to_i
				when 36
					send_command "", "request", "ssap://com.webos.service.ime/sendEnterKey", "{\"count\": 1}"
				when 51
					send_command "", "request", "ssap://com.webos.service.ime/deleteCharacters", "{\"count\": 1}"
				else
					send_command "", "request", "ssap://com.webos.service.ime/insertText", "{\"text\": \"#{key_str}\", \"replace\": 0}" unless key_str == "(null)"
				end
			end
		end
	end
	$pointer_ws.close
end

$parse_cb = nil
$quit_after_cmd = true
$cmd_count = 0
def send_command(prefix, type, uri, payload)
	$cmd_count += 1
	id = "#{prefix}#{$cmd_count.to_s}"
	$emitter.on id.to_sym do |data|
		$emitter.emit :response, data
	end
	cmd = "{\"id\":\"#{id}\",\"type\":\"#{type}\",\"uri\":\"#{uri}\"#{if payload then ",\"payload\":#{payload}}" else "}" end}"
	puts "\e[1;37mDEBUG\e[0m: >> #{cmd}" if $debug_msg
	$ws.send cmd
end

$emitter.on :error do |data|
	puts "\e[1;31mERROR!\e[0m #{data}"
	exit_code = 1
	$ws.close
end

def do_something(*cmd)
	case cmd[0]
	when "interactive"
		$quit_after_cmd = false
	when "message", "msg"
		send_command "", "request", "ssap://system.notifications/createToast", "{\"message\": \"#{cmd[1..-1] * " "}\"}"
	when "control"
		$quit_after_cmd = false
		send_command "", "request", "ssap://com.webos.service.networkinput/getPointerInputSocket", nil
		$parse_cb = method(:setup_pointer_ws)
		$quit_after_cmd = true
	when "off"
		send_command "", "request", "ssap://system/turnOff", nil
	when "info"
		send_command "sw_info_", "request", "ssap://com.webos.service.update/getCurrentSWInformation", nil
		$parse_cb = method(:parse_info)
	when "services"
		send_command "services_", "request", "ssap://api/getServiceList", nil
		$parse_cb = method(:parse_services)
	when "browse"
		unless cmd[1]
			puts "\e[1;31mERROR!\e[0m No URL passed!"
			$emitter.emit :error, "Invalid channel command!"
		end
		url = cmd[1]
		url = "http://#{url}" unless url =~ /^https?:\/\//
		send_command "", "request", "ssap://system.launcher/open", "{\"target\": \"#{url}\"}" 
	when "youtube", "yt"
		# Doesn't work on my TV, I don't know why
		# I just use the browser to view youtube videos
		unless cmd[1]
			puts "\e[1;31mERROR!\e[0m No URL passed!"
			$emitter.emit :error, "Invalid channel command!"
		end
		url = cmd[1]
		send_command "", "request", "ssap://system.launcher/launch", "{\"id\": \"youtube.leanback.v4\", \"params\": {\"contentTarget\": \"#{url}\"}}"
	when "volume"
		case cmd[1]
		when /^\d+$/
			send_command "", "request", "ssap://audio/setVolume", "{\"volume\": #{cmd[1]}}"
		when "up"
			send_command "volumeup_", "request", "ssap://audio/volumeUp", nil
		when "down"
			send_command "volumeup_", "request", "ssap://audio/volumeDown", nil
		else
			send_command "status_", "request", "ssap://audio/getVolume", nil
			$parse_cb = method(:parse_volume)
		end
	when "inputs"
		send_command "input_", "request", "ssap://tv/getExternalInputList", nil
		$parse_cb = method(:parse_inputs)
	when "input"
		send_command "", "request", "ssap://tv/switchInput", "{\"inputId\": \"#{cmd[1]}\"}"
	when "mute"
		if not cmd[1]
			send_command "", "request", "ssap://audio/setMute", "{\"mute\": true}"
		elsif cmd[1].downcase == "toggle"
			send_command "status_", "request", "ssap://audio/getStatus", nil
			$parse_cb = method(:toggle_mute)
		else
			send_command "", "request", "ssap://audio/setMute", "{\"mute\": #{str_to_bool(cmd[1]).to_s}}"
		end
	when "apps"
		send_command "launcher_", "request", "ssap://com.webos.applicationManager/listLaunchPoints", nil
		$parse_cb = method(:parse_apps)
	when "launch"
		send_command "", "request", "ssap://system.launcher/launch", "{\"id\": \"#{cmd[1]}\"}"
	when "close"
		send_command "", "request", "ssap://system.launcher/close", "{\"id\": \"#{cmd[1]}\"}"
	when "backspace"
		send_command "", "request", "ssap://com.webos.service.ime/deleteCharacters", "{\"count\": #{if cmd[1] then cmd[1].to_i.to_s else "1" end}}"
	when "enter"
		send_command "", "request", "ssap://com.webos.service.ime/sendEnterKey", nil
	when "pause"
		send_command "pause_", "request", "ssap://media.controls/pause", nil
	when "play"
		send_command "play_", "request", "ssap://media.controls/play", nil
	when "stop"
		send_command "stop_", "request", "ssap://media.controls/stop", nil
	when "channel"
		case cmd[1]
		when "list"
			# TODO: Get current channel
			# My TV isn't plugged into cabel
			# Unable to test these functions
			send_command "channels_", "request", "ssap://tv/getChannelList", nil 
			$parse_cb = method(:parse_channels)
		when "up"
			send_command "", "request", "ssap://tv/channelUp", nil
		when "down"
			send_command "", "request", "ssap://tv/channelDown", nil
		when "info"
			send_command "channels_", "request", "ssap://tv/getCurrentChannel", nil
		else
			send_command "", "request", "ssap://tv/openChannel", "{\"channelId\": \"#{cmd[2]}\"}"
		end
	when "media"
		case cmd[1]
		when "play"
			send_command "", "request", "ssap://media.controls/play", nil
		when "stop"
			send_command "", "request", "ssap://media.controls/stop", nil
		when "pause"
			send_command "", "request", "ssap://media.controls/pause", nil
		when "rewind"
			send_command "", "request", "ssap://media.controls/rewind", nil 
		when "forward"
			send_command "", "request", "ssap://media.controls/fastForward", nil
		else
			puts "\e[1;31mERROR!\e[0m Invalid arg: #{cmd[1]}"
			$emitter.emit :error, "Invalid media command!"
		end
	else
		puts "\e[1;31mERROR!\e[0m Invalid arg: #{cmd[0]}"
		$emitter.emit :error, "Invalid command!" if $quit_after_cmd
	end
end

$emitter.on :register_0 do |data|
	if data.has_key? "payload"
		if data["payload"].has_key? 'pairingType'
			puts "\e[1;37mWAITING FOR AUTH\e[0m: pairingType => #{data["payload"]["pairingType"]}" if $status_msg
		elsif data["payload"].has_key? 'client-key'
			puts "\e[1;32mAUTH ACCEPTED\e[0m: client-key => #{data['payload']['client-key']}" if $status_msg
			config["key"] = data['payload']['client-key']
			File.open path, "w" do |f|
				f.write config.to_json
			end

			do_something *$args
		end
	end
end

$emitter.on :response do |data|
	if data.has_key? "error"
		$emitter.emit :error, resp['error']
	else
		if data.has_key? 'payload' and (data['payload']['returnValue'] == true or data['payload']['subscribed'] == true)
			$parse_cb.call data if $parse_cb
			puts "\e[1;32mSUCCESS!\e[0m Command executed, exiting successfully" if $status_msg
			if $quit_after_cmd
				$ws.close
			else
				$parse_cb = nil
			end
		else
			$emitter.emit :error, "Failed to execute command!"
		end
	end
end

$ws.on :message do |msg|
	begin
		puts "\e[1;37mDEBUG\e[0m: << #{msg.data}" if $debug_msg
		resp = JSON.parse msg.data
		if resp.has_key? "error"
			$emitter.emit :error, resp['error']
		else
			$emitter.emit resp["id"].to_sym, resp
		end
	rescue JSON::ParserError => e
		$emitter.emit :error, "#{msg.data}, #{e.inspect}"
	end
end

$ws.on :open do
	puts "--- \e[1;37mCONNECTION OPENED\e[0m ---" if $status_msg
	$ws.send(if config and config.has_key? 'key' then hello.sub('CLIENTKEYGOESHERE', config['key']) else hello_new end)
end

$ws.on :close do |e|
	puts "--- \e[1;37mCONNECTION CLOSED\e[0m ---" if $status_msg
	exit exit_code
end

$ws.on :error do |e|
	$emitter.emit :error, e.inspect
end

at_exit do
	$ws.close if $ws
	$pointer_ws.close if $pointer_ws
	$sp.stop if $sp
	if $!.nil? || $!.is_a?(SystemExit) && $!.success?
		print "\e[1;32mSuccessful exit!\e[0!" if $status_msg
	else
		print "\e[1;31mFailure with code\e[0m: #{$!.is_a?(SystemExit) ? $!.status : 1}" if $status_msg
	end
end

trap "INT" do
	$sp.stop if $sp
	$pointer_ws.close if $pointer_ws
	$ws.close
end

loop do
	do_something *STDIN.gets.chomp.strip.split unless $quit_after_cmd
end
