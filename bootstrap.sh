#!/bin/bash

# echo $1 # np
# echo $2 # hostfile
# echo $3 # input_video
# echo $4 # output_video
# echo $5 # frame_rate

echo "Converting $3 into individual frames now..."

# Downscaling
ffmpeg -i $3 -vf "scale=1280:720" down_scaled.mp4
# Separate frames
ffmpeg -i down_scaled.mp4 -vf fps=$5 -vsync 0 video/input2/in%d.bmp
# Apply filter
make mpitest np=$1 hostfile=$2
# Stitch frames back together
ffmpeg -framerate $5 -i out%.bmp -c:v libx264 $4.mp4
