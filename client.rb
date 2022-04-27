

require 'socket'

s = TCPSocket.new 'localhost', 5008

s.write("hello from ruby \n");

s.each_line do |line|
    puts line
end

s.close