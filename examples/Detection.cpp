#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "jda/cascador.hpp"
#include "jda/common.hpp"
#include <time.h>

using namespace std;
using namespace cv;
using namespace jda;

int main(int argc, char *argv[]) {

  if (argc != 3) {
    cout << "Usage: " << argv[0] << " model_file image_file" << endl;
    return 0;
  }

  string model_file, image_file;
  model_file = argv[1];
  image_file = argv[2];
  cv::Mat color_image = imread(image_file, IMREAD_COLOR);

  if (color_image.empty()) {
    cout << "Load image failed" << endl;
    return 0;
  }

  JoinCascador joincascador;
  FILE *fd = fopen(model_file.c_str(), "rb");

  if (NULL == fd) {
    cout << "Open model failed." << endl;
    return 0;
  }

  joincascador.SerializeFrom(fd);
  fclose(fd);

  cv::Mat gray_image;
  cvtColor(color_image, gray_image, CV_BGR2GRAY);
  vector<double> scores;
  vector<Rect> rects;
  vector<Mat_<double> > shapes;
  DetectionStatisic statisic;
  clock_t start_time = clock();
  double t=(double)cvGetTickCount();
  joincascador.Detect(gray_image, rects, scores, shapes, statisic);
  t=((double)cvGetTickCount() - t)/(cvGetTickFrequency()*1000);
  cout << "OpenCV time is " << t <<"ms"<< endl;
  clock_t end_time = clock();
  cout << "Run time is " << static_cast<double>(end_time - start_time) /CLOCKS_PER_SEC*1000<<"ms"<< endl;
  const int face_total = rects.size();

  if (face_total <= 0) {
    cout << "Detection face is empty" << endl;
    return 0;
  }

  cout << "Detection: total " << face_total << " faces" << endl;


  for (int face_idx = 0; face_idx < face_total; ++face_idx) {
    cv::rectangle(color_image, rects[face_idx], cv::Scalar(0, 255, 0), 1);
    const Mat_<double> shape = shapes[face_idx];
    const int landmark_n = shape.cols / 2;
    cout << "Face:" << face_idx << ",score:" << scores[face_idx] << ",landmark_n:"
         << landmark_n << endl;

    for (int i = 0; i < landmark_n; ++i) {
      circle(color_image, Point(shape(0, 2 * i), shape(0, 2 * i + 1)), 1,
             cv::Scalar(0, 0, 255), -1);
    }
  }

  string save_path = image_file;
  save_path += "_jda.jpg";
  imwrite(save_path, color_image);
  return 0;
}
