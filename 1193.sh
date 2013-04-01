while :; do
    rand=$(echo $RANDOM | md5sum | cut -f1 -d' ')
    len=$(shuf -i 3-31 -n 1)
    try=${rand:$len}
    output=$(curl -s -X POST --data "hashable=$try" http://almamater.xkcd.com/?edu=usu.edu | grep -o 'It is off by .* bits.')
    echo "$try $output"
done | tee -a tries.txt
