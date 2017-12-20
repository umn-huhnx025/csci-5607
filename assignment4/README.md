# CSCI 5607 - Assignment 4
### By Jon Huhn (huhnx025)

## Demo Video
[YouTube link](https://youtu.be/jNYmxnUXYhw)

## Source Code
[Link](https://github.umn.edu/huhnx025/csci5607-assignment4/archive/v1.0.zip)

Compile and run with
```
make && bin/game
```
Note: GCC 7+ or any other compiler supporting C++17 is required.


## My Implementation
I based my implementation off of Prof. Guy's multi-textured-object example. It starts by parsing the map file and constructing a `Map` object, complete with `Wall`s, `Key`s, and `Door`s. Each of these map elements has a position and maybe some other data representing its state, like the color of a key or if a door is open. I also added walls around the perimeter of the map so players can't escape. Then, in the drawGeometry function, I loop through each of the map elements and draw them accordingly. To implement movement, I used the up and down keys to implement forward and backward movement and the left and right keys to rotate. Figuring out the math for each of these took quite a bit of trial and error. Later, I implemented full keyboard/mouse functionality, where the mouse looks around and the WASD keys move the camera. My implementation is totally first-person, so no character is rendered. Since I called my game A-Maze-Balls, I have my keys rendered as spheres and doors are cubes of the same color as its respective key. My walls use the brick texture and the ground plane uses a gravel texture I found online. I also added a white spinning cube to mark the goal of the maze. Once the goal is reached, a popup window opens saying you've won and closes the game.

## Issues
I had a number of issues getting started with OpenGL, but once I started playing around with Prof. Guy's example, OpenGL started to make a lot more sense. It took quite a bit of playing around to get the hang of rendering the right models with the right transformations, especially position. The same was also very true for the camera settings and movement. One of the biggest parts I had trouble with was collision detection. At first, I tried getting fancy and tried to find intersections between the triangle from the camera to the near plane of the view frustum and the square representing a wall or door. I found an algorithm that looked like it would work, but I tried implementing it and ran into a bunch of issues getting it to work. Instead, I check if the camera position is within the bounds of each wall, plus some threshold to account for the near plane and wall corners. I had to tweak these settings to try to find the best compromise between freedom of motion and not ending up inside walls.


## Extra Credit Tasks
- Texture map the walls and floors
- Integrated keyboard and mouse control (must support strafing/sidestepping)
- Animate the process of doors opening after they are unlocked
- Make a video of yourself playing, showcasing your features
