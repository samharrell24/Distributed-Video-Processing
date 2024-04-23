# Distributed-Video-Processing

Over the course of this independent study, I learned how to write C++, OpenMP and OpenMPI. I then used these new skills to create a distributed system that takes video, splits it up into individual frames, distributes those frames across my RPI cluster where they are processed. Each node in the RPI cluster then utilizes all threads available to it to speed up the process of applying the Sobel Filter. This program is a unique way to process videos by distributing the workload to other machines which is at the heart of distributed systems. 

## RPI Cluster

![picture_of_present](content/cluster.JPG)

### Original Image

![picture_of_present](content/box.bmp)

### Sobel Filter Applied to Image

![picture_of_sobel_present](/content/box_sobel.bmp)

The final output videos can be found in the content directory under the files: dining.mp4, snail.mp4, presents.mp4

### Executing Commands
```bash
./bootstrap.sh np hostfile content/input_video.mp4 output_video.mp4 frame_rate horizontal_pixels vertical_pixels

# Below is an example of what I use to process videos for example

./bootstrap.sh 4 hostfile content/vertical_dining_2160_3840_25fps.mp4 dining.mp4 25 720 1280
```