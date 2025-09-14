#!/usr/bin/env ruby

5.times do |i|
  puts "[STDOUT] ##{i}"
  sleep 0.125
  STDERR.puts "[STDERR] ##{i}"
  sleep 0.125
end
