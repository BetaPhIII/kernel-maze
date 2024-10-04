obj-m += raptor_maze.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
load:
	echo "osc" | sudo insmod raptor_maze.ko
reload:
	echo "osc" | sudo rmmod raptor_maze.ko
	echo "osc" | sudo insmod raptor_maze.ko
