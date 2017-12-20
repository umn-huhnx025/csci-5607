#!/usr/bin/env python3

import sys

base = """
camera {} {} {} {} {} {} 0 1 0 26

material 1 0 0 1 0 0 0 0 0 16 0 0 0 0
sphere {} {} {} 1

material 1 .5 0 1 .5 0 0 0 0 16 0 0 0 0
sphere -2 0 0 1
sphere 2 0 0 1

material 1 1 0 1 1 0 0 0 0 16 0 0 0 0
sphere -4 0 0 1
sphere 4 0 0 1

material 0 1 0 0 1 0 0 0 0 16 0 0 0 0
sphere -6 0 0 1
sphere 6 0 0 1

material 0 0 1 0 0 1 0 0 0 16 0 0 0 0
sphere -8 0 0 1
sphere 8 0 0 1


directional_light .7 .7 .7 1 -1 1


vertex 100 -1 100
vertex 100 -1 -100
vertex -100 -1 100
vertex -100 -1 -100


material .6 .6 .6 .6 .6 .6 0 0 0 16 0 0 0 1

triangle 0 1 2
triangle 1 2 3

ambient_light .2 .2 .2
background .1 .1 .1

output_image {}
"""

fps = float(sys.argv[2])
time_length = float(sys.argv[3])  # seconds
frames = fps * time_length


def lerp(start, end, part):
    return start + (end - start) * part


def get_sphere_position(frame):
    start = (0, 0, 6)
    end = (0, 0, 0)
    part = frame / frames
    return lerp(start[0], end[0], part), lerp(start[1], end[1], part), lerp(start[2], end[2], part)


def get_camera_position(frame):
    start = (-5, 0, -10)
    end = (-5, 0, -10)
    start_dir = (0.5, 0, 1)
    end_dir = (0.5, 0, 1)
    part = frame / frames
    return lerp(start[0], end[0], part), lerp(start[1], end[1], part), lerp(start[2], end[2], part), lerp(start_dir[0], end_dir[0], part), lerp(start_dir[1], end_dir[1], part), lerp(start_dir[2], end_dir[2], part)


def get_image_name(frame):
    return "frames/{}.png".format(frame)


def write_file(frame):
    filename = "frames/{}.scn".format(frame)
    print("Rendering frame {}/{}".format(frame + 1, int(frames)), end='\r')
    with open(filename, "w+") as scene_file:
        s_x, s_y, s_z = get_sphere_position(frame)
        c_x, c_y, c_z, c_dx, c_dy, c_dz = get_camera_position(frame)
        scene_file.write(base.format(
            c_x, c_y, c_z, c_dx, c_dy, c_dz, s_x, s_y, s_z, get_image_name(frame)))

write_file(int(sys.argv[1]))
