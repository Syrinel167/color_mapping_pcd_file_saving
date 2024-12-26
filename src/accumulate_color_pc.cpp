#include <ros/ros.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_ros/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_cloud.h>
#include <pcl/common/common.h>

class PointCloudAccumulator
{
public:
    PointCloudAccumulator()
    {
        // 创建一个空的全局点云
        global_cloud_.reset(new pcl::PointCloud<pcl::PointXYZRGB>);

        // 初始化订阅者
        sub_ = nh_.subscribe("/color_pc", 10, 
                             &PointCloudAccumulator::colorPcCallback, this);
        
        ROS_INFO("PointCloudAccumulator: Ready to accumulate point clouds.");
    }

    // 析构函数：节点退出时会自动调用
    ~PointCloudAccumulator()
    {
        saveAccumulatedCloud();
    }

private:
    // 回调函数：订阅 "/color_pc" 的点云消息
    void colorPcCallback(const sensor_msgs::PointCloud2ConstPtr &msg)
    {
        pcl::PointCloud<pcl::PointXYZRGB>::Ptr current_cloud(
            new pcl::PointCloud<pcl::PointXYZRGB>);

        // ROS -> PCL
        pcl::fromROSMsg(*msg, *current_cloud);

        // 累加到全局点云中
        *global_cloud_ += *current_cloud;

        ROS_INFO_STREAM("Received cloud size: " 
                        << current_cloud->size() 
                        << ", Global cloud size: " 
                        << global_cloud_->size());
    }

    // 在节点退出（析构）时，自动调用来保存一次PCD
    void saveAccumulatedCloud()
    {
        // 如果全局云为空，可以选择不保存
        if (global_cloud_->empty())
        {
            ROS_WARN("Global cloud is empty, nothing to save.");
            return;
        }

        // 设置保存文件名
        std::string filename = "accumulated_cloud.pcd";

        // 以ASCII或二进制的方式保存
        // ASCII示例：pcl::io::savePCDFileASCII(filename, *global_cloud_);
        // Binary示例：pcl::io::savePCDFileBinary(filename, *global_cloud_);

        if (pcl::io::savePCDFileBinary(filename, *global_cloud_) == 0)
        {
            ROS_INFO_STREAM("Saved accumulated point cloud: " 
                            << global_cloud_->size() 
                            << " points, to " << filename);
        }
        else
        {
            ROS_ERROR_STREAM("Failed to save " << filename);
        }
    }

private:
    ros::NodeHandle nh_;
    ros::Subscriber sub_;

    // 累加用的全局点云
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr global_cloud_;
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "accumulate_color_pc");

    // 用一个“作用域”把类实例包起来
    {
        PointCloudAccumulator accumulator;
        ros::spin(); // 等待订阅消息
    }
    // 当退出 spin、离开大括号时，会调用 accumulator 的析构函数 => 保存PCD

    return 0;
}
