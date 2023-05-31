// #include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv4/opencv2/highgui/highgui.hpp>
#include <ros/ros.h>
#include <sensor_msgs/image_encodings.h>
#include <sensor_msgs/CompressedImage.h>
#include <sensor_msgs/Image.h>
#include <std_msgs/Header.h>


cv_bridge::CvImagePtr cv_ptr;
cv::Mat tmp_image;
void imageCallback(const sensor_msgs::ImageConstPtr &msg)
{
  try
  {
    cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    tmp_image = cv_ptr->image.clone();
  }
  catch (cv_bridge::Exception &e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "compresser");
  ros::NodeHandle nh;

  ros::Subscriber sub_com = nh.subscribe("/camera/ctmp_imageolor/image_raw", 10, imageCallback);
  ros::Publisher pub_compressed = nh.advertise<sensor_msgs::CompressedImage>("camera/rgb_image/compressed", 10);

  while (nh.ok())
  {
    std_msgs::Header msg_h;
    msg_h.stamp = ros::Time::now();
    msg_h.frame_id = "camera_color_optical_frame";

    try
    {
      sensor_msgs::CompressedImage msg_com;

      if(tmp_image.empty())
      {
        continue;
      }

      bool val = cv::imencode(".jpg", tmp_image, msg_com.data);

      msg_com.header = msg_h;
      msg_com.format = "jpeg";

      
      if (pub_compressed.getNumSubscribers() > 0)
      {
        pub_compressed.publish(msg_com);
      }
    }
    catch (const std::exception &e)
    {
      std::cerr << e.what() << '\n';
    }catch (const std::runtime_error &e)
    {
      std::cerr << e.what() << '\n';
    }catch (const cv::Exception &e)
    {
      std::cerr << e.what() << '\n';
    }catch (const std::bad_alloc &e)
    {
      std::cerr << e.what() << '\n';
    }
  }
}
