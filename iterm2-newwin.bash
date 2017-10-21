#!/usr/bin/env bash

osascript -e "
tell application \"iTerm2\"
	set _window to (create window with default profile)
	tell _window
		tell current session
			write text \"$*; exit\"
		end tell
	end tell
end tell" &
