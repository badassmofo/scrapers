#!/usr/bin/env ruby

def walk_dirs dir, show_files, show_dots, indent = 0, miss_lines_d = [], is_last = 0, last_entries = false, miss_lines_f = []
  dirs, files = [], []
  Dir.entries(dir).each do |x|
    next if x == '.' or x == '..'
    next if x[0] == '.' and not show_dots
    y = "#{dir}/#{x}"
    File.directory?(y) ? dirs  << y : files << x
  end

  if indent > 0
    for j in 0..indent - 1
      unless j == 0
        print miss_lines_d.include?(j) ? ' ' : '│'
        print '  '
      end
    end

    print "#{is_last == 0 ? '└' : '├'}──"
    puts dir.split('/')[-1]
  end

  miss_lines_d.delete_if { |x| x > indent }

  miss_lines_f << indent - 1 if is_last == 0
  miss_lines_f.delete_if { |x| x > indent - 1 }

  miss_lines_d << indent if is_last == 0
  miss_lines_d.delete_if { |x| x > indent }

  if show_files
    files.sort.each do |x|
      for i in 0..indent - 1
        print i == 0 ? last_entries ? ' ' : '│' : miss_lines_f.include?(i) ? ' ' : '│'
        print '  '
      end

      puts "#{dirs.length > 0 ? '│ ' : ' '} #{x}"
    end
  end

  dirs.sort.each_with_index do |x, i|
    is_last_dir = dirs.length - 1 - i
    new_last_entries = true if is_last_dir == 0 and indent == 0 or last_entries

    walk_dirs x, show_files, show_dots,
                 indent + 1, miss_lines_d, is_last_dir, new_last_entries,miss_lines_f
  end
end

if __FILE__ == $0
  dirs = $*.uniq.select { |x| File.directory? x }
  dirs = [ Dir.pwd ] if dirs.empty?
  show_files, show_dots = $*.include?('/f'), $*.include?('/h')

  dirs.each_with_index do |x, i|
    puts if i > 0
    puts File.basename x if dirs.length > 1
    walk_dirs x, show_files, show_dots
  end
end
