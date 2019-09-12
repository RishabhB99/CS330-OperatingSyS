#include<types.h>
#include<context.h>
#include<file.h>
#include<lib.h>
#include<serial.h>
#include<entry.h>
#include<memory.h>
#include<fs.h>
#include<kbd.h>
#include<pipe.h>


/************************************************************************************/
/***************************Do Not Modify below Functions****************************/
/************************************************************************************/
void free_file_object(struct file *filep)
{
    if(filep)
    {
       os_page_free(OS_DS_REG ,filep);
       stats->file_objects--;
    }
}

struct file *alloc_file()
{
  
  struct file *file = (struct file *) os_page_alloc(OS_DS_REG); 
  file->fops = (struct fileops *) (file + sizeof(struct file)); 
  bzero((char *)file->fops, sizeof(struct fileops));
  stats->file_objects++;
  return file; 
}

static int do_read_kbd(struct file* filep, char * buff, u32 count)
{
  kbd_read(buff);
  return 1;
}

static int do_write_console(struct file* filep, char * buff, u32 count)
{
  struct exec_context *current = get_current_ctx();
  return do_write(current, (u64)buff, (u64)count);
}

struct file *create_standard_IO(int type)
{
  struct file *filep = alloc_file();
  filep->type = type;
  if(type == STDIN)
     filep->mode = O_READ;
  else
      filep->mode = O_WRITE;
  if(type == STDIN){
        filep->fops->read = do_read_kbd;
  }else{
        filep->fops->write = do_write_console;
  }
  filep->fops->close = generic_close;
  filep->ref_count = 1;
  return filep;
}

int open_standard_IO(struct exec_context *ctx, int type)
{
   int fd = type;
   struct file *filep = ctx->files[type];
   if(!filep){
        filep = create_standard_IO(type);
   }else{
         filep->ref_count++;
         fd = 3;
         while(ctx->files[fd])
             fd++; 
   }
   ctx->files[fd] = filep;
   return fd;
}
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/
/**********************************************************************************/



void do_file_fork(struct exec_context *child)
{
	for(int i=0;i<32;i++) //increasing ref_count of each open file object
	{
		if(child->files[i]!=NULL)
		{
			child->files[i]->ref_count+=1;
		}
	}

   /*TODO the child fds are a copy of the parent. Adjust the refcount*/
 
}

void do_file_exit(struct exec_context *ctx)
{
	for(int i=0;i<32;i++) //decreasing ref_count of each open file object
	{
		if(ctx->files[i]!=NULL)
		{
			
			if(ctx->files[i]->ref_count==1) //closing file if ref_count is 1
			generic_close(ctx->files[i]);

			ctx->files[i]->ref_count-=1;
		}
	}
  
   /*TODO the process is exiting. Adjust the ref_count
     of files*/
}

long generic_close(struct file *filep)
{
	if(filep->ref_count<0||filep==NULL) // if ref_count <0 or file = NULL return error
	{
    	int ret_fd = -EINVAL; 
    	return ret_fd;
	}
	filep->ref_count-=1; //decresing ref_count of file
	if(filep->ref_count==0){  //free file if ref_count is 0
		if(filep->type==PIPE)
			free_pipe_info(filep->pipe);
	    free_file_object(filep);
	}    
    return 1;
 
  /** TODO Implementation of close (pipe, file) based on the type 
   * Adjust the ref_count, free file object
   * Incase of Error return valid Error code 
   */
    int ret_fd = -EINVAL; 
    return ret_fd;
}

static int do_read_regular(struct file *filep, char * buff, u32 count)
{
  if(filep!=NULL) //read file if its valid and is in correct mode
  {
  	if(filep->offp+count>filep->inode->file_size)
  		return -EINVAL;
    if(filep->mode==0x1||filep->mode==0x3||filep->mode==0x5||filep->mode==0x7){
    int size = flat_read(filep->inode,buff,count,&(filep->offp));
    filep->offp+=count;     
    return size;
    }
    else
      return -EACCES; //access error otherwise
  }
   /** TODO Implementation of File Read, 
    *  You should be reading the content from File using file system read function call and fill the buf
    *  Validate the permission, file existence, Max length etc
    *  Incase of Error return valid Error code 
    * */
    int ret_fd = -EINVAL; 
    return ret_fd;
}


static int do_write_regular(struct file *filep, char * buff, u32 count)
{
  if(filep!=NULL) //write file if its valid and is in correct mode
  {
    if(filep->mode==0x2||filep->mode==0x3||filep->mode==0x6||filep->mode==0x7){
      int size = flat_write(filep->inode,buff,count,&(filep->offp));
      filep->offp+=count;
      return size;
    }
    else
      return -EACCES; //access error otherwise
  }
    /** TODO Implementation of Fil]=e write, 
    *   You should be writing the content from buff to File by using File system write function
    *   Validate the permission, file existence, Max length etc
    *   Incase of Error return valid Error code 
    * */
    int ret_fd = -EINVAL; 
    return ret_fd;
}

static long do_lseek_regular(struct file *filep, long offset, int whence)
{
	if(filep==NULL) //if file is null return error
	{
		return -EOTHERS;
	}
  if(whence==0) //if whence is seek set
  {
  	if(offset<=filep->inode->file_size&&offset>=0){
	    filep->offp = offset;
	    return filep->offp;
	}  
	else
	return -EINVAL;  
  }
  else if(whence==1) //if whence is seek curr
  {
  	if(offset+filep->offp<=filep->inode->file_size&&offset+filep->offp>=0){
	    filep->offp += offset;
	    return filep->offp; 
	}  
	else
	return -EINVAL;
  }
  else if(whence==2) //if whence is seek end
  {
  	if(offset<=0&&filep->inode->max_pos+offset>=0){
  		filep->offp = filep->inode->file_size+offset;
  		return filep->offp;
  	}  
	else
	return -EINVAL;
  }
    /** TODO Implementation of lseek 
    *   Set, Adjust the ofset based on the whence
    *   Incase of Error return valid Error code 
    * */
    int ret_fd = -EINVAL; 
    return ret_fd;
}


extern int do_regular_file_open(struct exec_context *ctx, char* filename, u64 flags, u64 mode)
{ 
  struct inode* p;
  if(flags>=O_CREAT) //create file and check if flag matches mode
  {
  	if(flags==(O_CREAT|O_WRITE))
	{
		if(mode!=(O_WRITE))
			return -EINVAL;
	}
	else if(flags==(O_CREAT|O_READ))
	{
		if(mode!=(O_READ))
			return -EINVAL;	
	}
	else if(flags==(O_CREAT|O_EXEC))
	{
		if(mode!=(O_EXEC))
			return -EINVAL;
	}
	else if(flags==(O_CREAT|O_RDWR))
	{
		if(mode!=(O_RDWR)&&mode!=(O_WRITE)&&mode!=(O_READ))
			return -EINVAL;
	}
	else if(flags==(O_CREAT|O_WRITE|O_EXEC))
	{
		if(mode!=(O_WRITE|O_EXEC)&&mode!=(O_WRITE)&&mode!=(O_EXEC))
			return -EINVAL;
	}
	else if(flags==(O_CREAT|O_READ|O_EXEC))
	{
		if(mode!=(O_READ|O_EXEC)&&mode!=(O_EXEC)&&mode!=(O_READ))
			return -EINVAL;
	}
	else if(flags==(O_CREAT))
		return -EINVAL;
	p = create_inode(filename,flags-(O_CREAT));
  }
  else{
  	p = lookup_inode(filename);
  	if(p==NULL)
  		return -EINVAL;
  }
    if(p->mode==(O_WRITE)) // check mode of file and mode given by user
	{
		if(mode!=(O_WRITE))
			return -EINVAL;
	}
	else if(p->mode==(O_READ))
	{
		if(mode!=(O_READ))
			return -EINVAL;	
	}
	else if(p->mode==(O_EXEC))
	{
		if(mode!=(O_EXEC))
			return -EINVAL;
	}
	else if(p->mode==(O_WRITE|O_READ))
	{
		if(mode!=(O_RDWR)&&mode!=(O_WRITE)&&mode!=(O_READ))
			return -EINVAL;
	}
	else if(p->mode==(O_WRITE|O_EXEC))
	{
		if(mode!=(O_WRITE|O_EXEC)&&mode!=(O_WRITE)&&mode!=(O_EXEC))
			return -EINVAL;
	}
	else if(p->mode==(O_READ|O_EXEC))
	{
		if(mode!=(O_READ|O_EXEC)&&mode!=(O_EXEC)&&mode!=(O_READ))
			return -EINVAL;
	}
  if(p!=NULL)
  {
    for(int i=3;i<32;i++) //finding empty file descripter
    {
      if(ctx->files[i]==NULL)
      {
        ctx->files[i] = alloc_file(); // filling struct file
        ctx->files[i]->type = REGULAR;
	    ctx->files[i]->mode=mode;
	    ctx->files[i]->inode=p;
	    int (*read)(struct file*,char*,u32)= &do_read_regular;
	    int (*write)(struct file*,char*,u32)= &do_write_regular;
	    long (*close)(struct file*)= &generic_close;
	    long (*lseek)(struct file*,long,int)=&do_lseek_regular;
	    ctx->files[i]->fops->read = read;
	    ctx->files[i]->fops->write = write;
	    ctx->files[i]->fops->close = close;
	    ctx->files[i]->fops->lseek = lseek;
        ctx->files[i]->ref_count=1;
        return i;
      }
    }
  }
  /**  TODO Implementation of file open, 
    *  You should be creating file(use the alloc_file function to creat file), 
    *  To create or Get inode use File system function calls, 
    *  Handle mode and flags 
    *  Validate file existence, Max File count is 32, Max Size is 4KB, etc
    *  Incase of Error return valid Error code 
    * */
    int ret_fd = -EINVAL; 
    return ret_fd;
}

int fd_dup(struct exec_context *current, int oldfd)
{
  if(oldfd>31||oldfd<0||current->files[oldfd]==NULL) //return error on invalid input
    return -EINVAL;

  for(int i=0;i<32;i++) //finding empty fd
  {
    if(current->files[i]==NULL)
    {
      current->files[oldfd]->ref_count+=1;
      current->files[i]=current->files[oldfd];

      return i;
    }
  }
     /** TODO Implementation of dup 
      *  Read the man page of dup and implement accordingly 
      *  return the file descriptor,
      *  Incase of Error return valid Error code 
      * */
    int ret_fd = -ENOMEM; 
    return ret_fd;
}


int fd_dup2(struct exec_context *current, int oldfd, int newfd)
{
  if(newfd>31||oldfd>31||newfd<0||oldfd<0||current->files[oldfd]==NULL) //return error on invalid input
    return -EINVAL;

  if(current->files[newfd]!=NULL) //empty newfd if nonempty
  if(generic_close(current->files[newfd])!=1)
    return -EINVAL;

  current->files[oldfd]->ref_count+=1;
  current->files[newfd] = current->files[oldfd];
  return newfd;
  /** TODO Implementation of the dup2 
    *  Read the man page of dup2 and implement accordingly 
    *  return the file descriptor,
    *  Incase of Error return valid Error code 
    * */
    /*int ret_fd = -EINVAL; 
    return ret_fd;*/
}
