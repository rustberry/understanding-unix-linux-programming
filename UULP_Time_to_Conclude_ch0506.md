# Time to Conclude!

## Ch 5 6

1. `fcntl` vs `tcsetaatr`. 
2. `icanon` vs `O_NDELAY` mode.
3. Only after writing code had I formed a clear image of how to use those sys calls.

### `fcntl` vs `tcsetaatr`

`fcntl` edits flag. `tcsetaatr` edits and saves back a  `termios` `struct`. Demo for quick recall:

> get setting -> edit -> save setting back

```c
void set_cr_noecho_mode()
/* 
 * purpose: put file descriptor 0 into chr-by-chr mode and noecho mode
 *  method: use bits in termios
 */
{
	struct	termios	ttystate;

	tcgetattr( 0, &ttystate);		/* read curr. setting	*/
	ttystate.c_lflag    	&= ~ICANON;	/* no buffering		*/
	ttystate.c_lflag    	&= ~ECHO;	/* no echo either	*/
	ttystate.c_cc[VMIN]  	=  1;		/* get 1 char at a time	*/
	tcsetattr( 0 , TCSANOW, &ttystate);	/* install settings	*/
}
```

> get flag -> edit -> F_SETFL (save back)

```c
void set_nodelay_mode()
/*
 * purpose: put file descriptor 0 into no-delay mode
 *  method: use fcntl to set bits
 *   notes: tcsetattr() will do something similar, but it is complicated
 */
{
	int	termflags;

	termflags = fcntl(0, F_GETFL);		/* read curr. settings	*/
	termflags |= O_NDELAY;			/* flip on nodelay bit	*/
	fcntl(0, F_SETFL, termflags);		/* and install 'em	*/
}
```

// TODO: why using`tcsetattr()` to edit `O_NDELAY` is complicated.





### `icanon` vs `O_NDELAY` mode.

> classification: 
> `icanon` here is "The setting of the **ICANON** canon flag in *c_lflag*." 

`icanon` is one of the three terminal input modes, in which the input is read line by line.

`O_NDELAY` is a flag of a `fd`(file descriptor). When set, `read` from the `fd` would not wait input: if the buffer is empty, `read` will just return `0`, just as the case where `EOF` is met.

