add-auto-load-safe-path ./
set confirm off

define bs
make all -j10
start
end

source ./lib/scripts/pretty/aoc_pretty_printers.py

