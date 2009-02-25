@echo off
@if not "%~d0" == "~d0" goto WinNT
\bin\ruby -x "/bin/testrb.bat" %1 %2 %3 %4 %5 %6 %7 %8 %9
@goto endofruby
:WinNT
"%~dp0ruby" -x "%~f0" %*
@goto endofruby
#!/bin/ruby
require 'test/unit'
(r = Test::Unit::AutoRunner.new(true)).process_args(ARGV) or
  abort r.options.banner + " tests..."
exit r.run
__END__
:endofruby
