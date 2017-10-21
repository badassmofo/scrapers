-- Adapted from https://github.com/rohieb/mpv-notify

function notify_current_track()
  path  = mp.get_property_native("path")
  data  = mp.get_property_native("metadata")
  cover = (path ~= nil and "cover.jpg" or path:match("(.*/)") .. "cover.jpg")

  function has_value (tab, val)
    for index, value in ipairs (tab) do
      if value == val then
        return true
      end
    end
    return false
  end

  if not has_value({'.aac', '.flac', '.m4a', '.m4p', '.mp3', '.ogg', '.opus', '.wav'}, path:match("^.+(%..+)$")) then
    do return end
  end

  function get_metadata(key)
    for k, v in pairs({key, key:upper()}) do
      if data[v] and string.len(data[v]) > 0 then
        return data[v]
      end
    end
    return "N/A"
  end

  function file_exists(name)
    local f=io.open(name,"r")
    if f~=nil then io.close(f) return true else return false end
  end

  os.execute("terminal-notifier -message \"" .. get_metadata("album") .. " - " .. get_metadata("track") .. "\" -title \"mpv-notify\" -subtitle \"" .. get_metadata("artist") .. " - " ..  get_metadata("title") .. "\" -appIcon \"/usr/local/Cellar/mpv/HEAD/mpv.app/Contents/Resources/icon.icns\" -contentImage \"" .. (file_exists(cover) and cover or "") .. "\" -execute \"pkill mpv\"")
end

mp.register_event("file-loaded", notify_current_track)
