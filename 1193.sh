# SUBSTITUE 20000 WITH A RANDOM NUMBER BETWEEN 1-400000
# When it finishes, choose a new number...
for try in $(tac /usr/share/dict/words | tail -n +20000 | head -10000); do
    output=$(curl -s -X POST --data "hashable=${try}" http://almamater.xkcd.com/?edu=usu.edu | grep -o 'It is off by .* bits.')
    echo "$try $output"
done | tee -a tries.txt