#!/usr/bin/env osascript

tell application "iTerm2"
  tell current window
    tell current session
      set newSession to (split horizontally with default profile)
    end tell
  end tell
end tell
