#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/random.h>
#include <linux/kernel.h>
#include <linux/slab.h>

// Module metadata
MODULE_AUTHOR("Bryon Catlin III");
MODULE_DESCRIPTION("Maze program to generate maze on read of function based on user input size");
MODULE_LICENSE("GPL");

static struct proc_dir_entry* proc_entry;
static char maze_dimensions[6];
/*
Name: Bryon Catlin III
Date: 09/23/2024
Description: Generate random number function
*/
int get_random_num(void){
 int i, rand_num;
 get_random_bytes(&i, sizeof(i));
 rand_num = abs(i%4);
 return rand_num;
}

/*
Name: Bryon Catlin III
Date: 09/23/2024
Description: Create empty array which is the size of the maze
*/
void maze_create(char maze[], int size, int height, int width){
 int i;
 for(i = 0; i<size; i++){
  maze[i]='#';
 }
}
/*
Name: Bryon Catlin III
Date: 09/23/2024
Description: Direction function to swap path direction randomly
*/
void change_direction(int directions[4][2]){
 int i, tempX, tempY, r; // initialize variables
 for(i=0; i<4; i++){
  r = get_random_num();
  tempX = directions[i][0], tempY = directions [i][1];
  directions[i][0] = directions[r][0];
  directions[i][1] = directions[r][1];
  directions[r][0] = tempX;
  directions[r][1] = tempY;
 }
}

/*
Name: Bryon Catlin III
Date: 09/23/2024
Description: Creates path within the maze array
*/
void create_path(char maze[], int x, int y, int height, int width){
 int i,direction_x,direction_y,next_x,next_y;
 int directions[4][2] = {{0,2}, {2,0}, {0,-2}, {-2,0}};
 change_direction(directions);
 for(i = 0; i<4; i++){
  direction_x = directions[i][0]; //declare directions for x-axis
  direction_y = directions[i][1]; //declare directions for y-axis
  next_x = x+direction_x;
  next_y = y+direction_y;

  if (next_x > 0 && next_x < height && next_y > 0 && next_y < width && maze[next_x * (width + 1) + next_y] == '#'){   
   maze[(x+direction_x/2)*(width+1)+(y+direction_y/2)]=' ';
   maze[next_x*(width+1)+next_y] = ' ';
   create_path(maze,next_x,next_y, height, width); //call recursively
  }
 }
}
/*
Name: Bryon Catlin III
Date: 09/25/2024
Description: Add newline characters to the end of each row on completed maze
*/
void add_new_lines(char maze[], int width, int height) {
 int new_size = (width + 1) * height;  // calculate new size including \n chars
 char *new_maze = kmalloc(new_size, GFP_KERNEL); // allocate new mazze in memory
 int i, j;
 // add \n characters for new lines to the end of each row
 for (i = 0, j = 0; i < height; i++) {
  memcpy(&new_maze[j], &maze[i * width], width); // copy one row
  j += width;
  new_maze[j] = '\n'; // add \n char to new array
  j++;
 }

 memcpy(maze, new_maze, new_size); // overwrite original array with new_maze
 kfree(new_maze); // free memory before completion
}
/*
Name: Bryon Catlin III
Date: 09/22/2024
Description: Read function to create and display maze
*/
static ssize_t read(struct file* file, char __user* user_buffer, size_t count, loff_t* offset)
{
 int width, height, maze_length, maze_size; // initialize variables used
 sscanf(maze_dimensions, "%d %d", &width, &height); // read width and height into maze_dimensions array
 maze_size = (width+3)*(height+2)+1; // reserve memory for maze, including the surrounding wall
 char *maze = kmalloc(maze_size, GFP_KERNEL); // allocate memory for maze so it can be of variable size
 memset(maze, '#', maze_size);
 printk(KERN_INFO "Read function: raptor_maze\n");
 maze_create(maze, maze_size, height+1, width+1); // call maze_create function to initialize the maze
 maze[1*(width+2)+1]=' '; // start path creation at position (1,1)
 create_path(maze, 1, 1, height+1, width+1); // call function to modify maze with random path
 add_new_lines(maze, width+2, height+2); // call function to add new lines at end of every line
 maze[((width+2)*(height+3))-1] = '\0';
 maze_length = strlen(maze);// find length of maze to send through user_buffer
 printk(KERN_INFO "%d", maze_length);
 if (*offset > 0){
  kfree(maze); // deallocate memory when program completes
  return 0;
 }
 if(copy_to_user(user_buffer, maze, maze_length)){
  kfree(maze); // dellaocate memory if error in copying data to user_buffeer
  return -EFAULT;
 }
 *offset = maze_length;
 kfree(maze); // deallocate memory before return
 return maze_length;
}

/*
Name: Bryon Catlin III
Date: 09/25/2024
Description: Write function to receive user input for dimensions
*/
static ssize_t write(struct file *filp, const char __user *buffer, size_t count, loff_t *pos) {
 char temp[6];  // reserve buffer for large array to allow integer loading

 // check to make sure copy succeeds
 if (copy_from_user(temp, buffer, count)) { 
  printk(KERN_ERR "Error copying data from user space.\n");
  return -EFAULT; 
 }
 // set the null character for the end of the array
 temp[5] = '\0';
 
 // copy the array into maze_dimensions variable
 strncpy(maze_dimensions, temp, 6);
 printk(KERN_INFO "Received input: %s\n", maze_dimensions);
 return count;
}

static struct file_operations fops =
{
 .owner = THIS_MODULE,
 .read = read,
 .write = write
};

// init and exit methods
/*
Name: Bryon Catlin III
Date: 09/22/2024
Description: Init function
*/
static int __init my_init(void) {
 proc_entry = proc_create("raptor_maze", 0666, NULL, &fops);
 printk(KERN_INFO "Raptor maze loaded.");
 return 0;
}

/*
Name: Bryon Catlin III
Date: 09/22/2024
Description: Exit function
*/
static void __exit my_exit(void) {
 proc_remove(proc_entry);
 printk(KERN_INFO "RARWRRRRRR I'M A RAPTOR ESCAPE MY MAZE...");
}
module_init(my_init);
module_exit(my_exit);
