Can work with [FAST-LIO-COLOR-MAPPING](https://github.com/YWL0720/FAST-LIO-COLOR-MAPPING)

# Build and Run

```bash
cd YOUR_WORKSPACE/src
git clone https://github.com/Syrinel167/color_mapping_pcd_file_saving.git
cd ..
catkin_make
source devel/setup.bash
# roscore at another terminal
rosrun color_map accumulate_color_pc
```

After completing your SLAM process, press `Ctrl + C` to stop the node. The `.pcd` file will be saved in the current directory, where you can view or process it further.
