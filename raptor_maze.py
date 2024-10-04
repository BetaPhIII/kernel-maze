width = 100
height = 100
while width>27 or height>27:
 width = int(raw_input("Enter dimension for width (Max 27):"))
 height = int(raw_input("Enter dimension for height (Max 27):"))
 if(width>28 or height>28):
  print("Maximum size is 28x28. Please reenter your values.")

user_input = "{}x{}".format(width,height)
kernel_module = open('/proc/raptor_maze', 'w')
kernel_module.write(user_input)
kernel_module.close()

kernel_module = open('/proc/raptor_maze')
lines = kernel_module.readlines()
for line in lines:
	print line,
kernel_module.close()

