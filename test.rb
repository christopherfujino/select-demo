#!/usr/bin/env ruby

5.times do |i|
  STDOUT.puts "[STDOUT] ##{i}"
  STDOUT.flush
  sleep 0.125
  STDERR.puts "[STDERR] ##{i}"
  STDERR.flush
  sleep 0.125
end
