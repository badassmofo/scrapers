printf = function(s, ...)
  return io.write(s:format(...))
end

function on_connect()
  print("CONNECTED!")
  send("HELLO!")
end

function on_error(msg)
  printf("ERROR: %s at %s\n", msg, os.date())
end

function on_exit()
  print("GOODBYE!")
end

function on_msg(msg)
  print(msg)
end
