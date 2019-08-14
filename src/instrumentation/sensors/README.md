
# How to use positional sensors with the dataviewer

Gdv provides an executable as well: the so called dataviwer. It can be used to
provide a graphical representation of the positional data from the IIO-devices
of your PC (see `/sys/bus/iio/devices/`).

To read out the value and pipe it into a file-descriptor execute in bash:
`mkfifo tmp_fifo `
`while sleep 0.1; do cat /sys/bus/iio/devices/iio\:device1/in_incli_x_raw | awk '{temp =  $1; printf("%c", int(temp*256*0.000174532/3.141592654))}' > tmp_fifo; done`
`while sleep 0.05; do cat /sys/bus/iio/devices/iio\:device1/in_incli_x_raw | awk '{temp = 0.000174532 * $1; printf("%c", temp)}' > tmp_fifo; done`

