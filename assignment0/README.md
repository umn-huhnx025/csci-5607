### CSCI 5607 - Fundamentals of Computer Graphics 1
### Assignment 0
### Jon Huhn (huhnx025)


## Program in Progress
![In Progress](images/in_progress.gif)

## Difficulties
I initially started with a much different approach to the transformations that Prof. Guy discussed in lecture. Much of mine had to do with calculating the lines that make up the edges of the square and testing the mouse's position with respect to those lines. I found Prof. Guy's approach much easier, so I implemented that instead. I still struggled a little bit with rotation, but eventually found some help from a [StackOverflow answer](https://stackoverflow.com/questions/2259476/rotating-a-point-about-another-point-2d).

Another piece I had trouble with was smoothing out the transition when a user clicked inside the square, but not its center. Specifically, I was having trouble figuring out how to redraw the screen after each intermediate point was calculated. I abstracted away the screen redraw logic into its own function and called that from the `mouseClicked` function.

## Extra Features
- Pressing space will change the color of the square
- `square-wide` and `square-narrow` binaries are also provided, which implement the square in a wide and narrow window. Dynamic window resizing does not work however.

## Program Source
[Source code](https://github.umn.edu/huhnx025/csci5607-assignment0/archive/v1.0.zip)

[Mac Executables](https://github.umn.edu/huhnx025/csci5607-assignment0/releases/download/v1.0/square-mac.zip)

[Linux Executables](https://github.umn.edu/huhnx025/csci5607-assignment0/releases/download/v1.0/square-linux.zip)

Compile with:
```
make
```

Run with:
```
./square
```
