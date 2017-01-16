#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "jda/cascador.hpp"
#include "jda/common.hpp"

using namespace std;
using namespace cv;
using namespace jda;

int main(int argc, char *argv[]) {

  if (argc != 4) {
    cout << "Usage: " << argv[0] << " deploy_file model_file image_file" << endl;
    return 0;
  }

  cout << "Hello world" << endl;
  cv::Mat color_image = imread(argv[3], IMREAD_COLOR);

  if (color_image.empty()) {
    cout << "Load image failed" << endl;
    return 0;
  }

  cout << "Load image success\n";
  JoinCascador joincascador();
//  cout << "Start fopen model"<<endl;
//  FILE *fd = fopen(argv[2], "rb");

//  if (NULL == fd) {
//    cout << "Open model failed." << endl;
//    return 0;
//  }

//  cout << "Open model success\n";
//  joincascador.SerializeFrom(fd);
//  fclose(fd);
//  cout << "test 1" << endl;

//  cv::Mat gray_image;
//  cvtColor(color_image, gray_image, CV_BGR2GRAY);
//  vector<double> scores;
//  vector<Rect> rects;
//  vector<Mat_<double> > shapes;
//  DetectionStatisic statisic;
//  joincascador.Detect(gray_image, rects, scores, shapes, statisic);

//  const int face_total = rects.size();

//  if (face_total <= 0) {
//    cout << "Detection face is empty" << endl;
//    return 0;
//  }

//  cout << "Detection: total " << face_total << " faces" << endl;


//  for (int face_idx = 0; face_idx < face_total; ++face_idx) {
//    cv::rectangle(color_image, rects[face_idx], cv::Scalar(0, 255, 0), 1);
//    const Mat_<double> shape = shapes[face_idx];
//    const int landmark_n = shape.cols / 2;

//    for (int i = 0; i < landmark_n; ++i) {
//      circle(color_image, Point(shape(0, 2 * i), shape(0, 2 * i + 1)), 1,
//             cv::Scalar(0, 0, 255), -1);
//    }
//  }

//  string save_path = argv[3];
//  save_path += "_jda.jpg";
//  imwrite(save_path, color_image);
  return 0;
}
