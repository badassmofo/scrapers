printf = function(s, ...)
  return io.write(s:format(...))
end

function string.starts(s, start)
  return string.sub(s, 1, string.len(start)) == start
end

function justinfan()
  local s = "justinfan"
  for i = 1, 9 do
    s = s .. string.char(math.random(48, 57))
  end
  return s
end

function on_connect()
  local user = justinfan()
  local chan = "badassmofofoo"
  send(string.format("USER %s 0 0 :%s", user, user))
  send(string.format("NICK %s", user))
  send(string.format("CAP REQ :twitch.tv/membership"))
  send(string.format("CAP REQ :twitch.tv/tags"))
  send(string.format("CAP REQ :twitch.tv/commands"))
  send(string.format("JOIN #%s", chan))
end

function on_error(msg)
  printf("ERROR: %s at %s\n", msg, os.date())
end

function on_exit()
  print("GOODBYE!")
end

function on_msg(msg)
  if string.starts(msg, "PING") then
    send(("%s%s%s"):format(msg:sub(1, 1), 'O', msg:sub(3)))
  elseif string.starts(msg, "@badge") then
    print(msg)
  end
end
