#!/bin/bash

# List of arg parameters to pass in
# echo $1 # np
# echo $2 # hostfile
# echo $3 # input_video
# echo $4 # output_video
# echo $5 # frame_rates
# echo $6 # horizontal scale
# echo $7 # vertical scale

GREEN='\033[32;40;1m'
NC='\033[0m'

# Deleting old files to be able to process a video
rm video/input/* > /dev/null 2>&1
rm video/output/* > /dev/null 2>&1
rm down_scaled.mp4 > /dev/null 2>&1

echo -e "${GREEN}Downscaling video to increase speed...${NC}\n"
# Downscaling
ffmpeg -i $3 -vf "scale=$6:$7" down_scaled.mp4 > /dev/null 2>&1
# # Separate frames
echo -e "${GREEN}Converting $3 into individual frames now...${NC}\n"
ffmpeg -i down_scaled.mp4 -vf fps=$5 -vsync 0 video/input/in%d.bmp > /dev/null 2>&1
num_frames=$(find video/input -type f -name "*.bmp" | wc -l)
echo -e "${GREEN}Created $num_frames total frames from $3...${NC}\n"
# #Applying Sobel Filter
echo -e "${GREEN}Running Sobel Filter across Cluster...${NC}\n$"
time make mpitest np=$1 hostfile=$2
echo -e "${GREEN}SOBEL FILTER COMPLETE${NC}\n"
# # Stitch frames back together
echo -e "${GREEN}Stitching frames back together with FFMPEG...${NC}\n"
ffmpeg -framerate $5 -i video/output/out%d.bmp -c:v libx264 $4 > /dev/null 2>&1
echo -e "${GREEN}$4 has been created. Goodbye :)${NC}\n"