#include <omp.h>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <jsmn.hpp>
#include "jda/common.hpp"
#include <iostream>

using namespace cv;
using namespace std;
using namespace jsmn;

namespace jda {

void LOG(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char msg[256];
  vsprintf(msg, fmt, args);
  va_end(args);

  char buff[256];
  time_t t = time(NULL);
  strftime(buff, sizeof(buff), "[%x - %X]", localtime(&t));
  printf("%s %s\n", buff, msg);
}

void dieWithMsg(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  char msg[256];
  vsprintf(msg, fmt, args);
  va_end(args);

  LOG(msg);
  exit(-1);
}

double calcMeanError(const vector<Mat_<double> >& gt_shapes, \
                     const vector<Mat_<double> >& current_shapes) {
  const Config& c = Config::GetInstance();
  const int N = gt_shapes.size();
  const int landmark_n = c.landmark_n;
  double e = 0.;
  Mat_<double> delta_shape;
  for (int i = 0; i < N; i++) {
    double left_x, left_y, right_x, right_y;
    left_x = left_y = right_x = right_y = 0.;
    for (int j = 0; j < c.left_pupils.size(); j++) {
      left_x += gt_shapes[i](0, 2 * c.left_pupils[j]);
      left_y += gt_shapes[i](0, 2 * c.left_pupils[j] + 1);
    }
    left_x /= c.left_pupils.size();
    left_y /= c.left_pupils.size();
    for (int j = 0; j < c.right_pupils.size(); j++) {
      right_x += gt_shapes[i](0, 2 * c.right_pupils[j]);
      right_y += gt_shapes[i](0, 2 * c.right_pupils[j] + 1);
    }
    right_x /= c.right_pupils.size();
    right_y /= c.right_pupils.size();
    double pupil_dis;
    pupil_dis = std::sqrt(std::pow(left_x - right_x, 2) + \
                std::pow(left_y - right_y, 2));

    delta_shape = gt_shapes[i] - current_shapes[i];
    double e_ = 0.;
    for (int j = 0; j < landmark_n; j++) {
      e_ += std::sqrt(std::pow(delta_shape(0, 2 * j), 2) + \
                      std::pow(delta_shape(0, 2 * j + 1), 2));
    }
    e += e_ / pupil_dis;
  }
  e /= landmark_n * N;
  return e;
}

Mat drawShape(const Mat& img, const Mat_<double>& shape) {
  Mat img_ = img.clone();
  const int landmark_n = shape.cols / 2;
  Scalar color;
  if (img_.type() == CV_8UC1) {
    color = Scalar(255, 255, 255);
  }
  else {
    color = Scalar(0, 255, 0);
  }
  for (int i = 0; i < landmark_n; i++) {
    circle(img_, Point(shape(0, 2 * i), shape(0, 2 * i + 1)), 1, color, -1);
  }
  return img_;
}
Mat drawShape(const Mat& img, const Mat_<double>& shape, const Rect& bbox) {
  Mat img_ = img.clone();
  const int landmark_n = shape.cols / 2;
  rectangle(img_, bbox, Scalar(0, 0, 255), 2);
  Scalar color;
  if (img_.type() == CV_8UC1) {
    color = Scalar(255, 255, 255);
  }
  else {
    color = Scalar(0, 255, 0);
  }
  for (int i = 0; i < landmark_n; i++) {
    circle(img_, Point(shape(0, 2 * i), shape(0, 2 * i + 1)), 1, color, -1);
  }
  return img_;
}

void showImage(const Mat& img) {
  cv::imshow("img", img);
  cv::waitKey(0);
}

Config::Config() {
  jsmn::Object json_config = jsmn::parse("/home/dxg/work_place/JDA/model/config.json");
  // model meta data
  T = json_config["T"].unwrap<Number>();
  K = json_config["K"].unwrap<Number>();
  landmark_n = json_config["landmark_n"].unwrap<Number>();
  tree_depth = json_config["tree_depth"].unwrap<Number>();
  shift_size = json_config["random_shift"].unwrap<Number>();

  // image size
  jsmn::Object& image_size_config = json_config["image_size"].unwrap<Object>();
  multi_scale = image_size_config["multi_scale"].unwrap<Boolean>();
  img_o_size = image_size_config["origin_size"].unwrap<Number>();
  img_h_size = image_size_config["half_size"].unwrap<Number>();
  img_q_size = image_size_config["quarter_size"].unwrap<Number>();

  // hard negative mining
  jsmn::Object& mining_config = json_config["hard_negative_mining"].unwrap<Object>();
  mining_factor = mining_config["factor"].unwrap<Number>();
  mining_min_size = mining_config["min_size"].unwrap<Number>();
  mining_step_ratio = mining_config["step_ratio"].unwrap<Number>();
  jsmn::Array& mining_ths = mining_config["mining_th"].unwrap<Array>();
  mining_th.resize(mining_ths.size());
  for (int i = 0; i < mining_ths.size(); i++) {
    mining_th[i] = mining_ths[i].unwrap<Number>();
  }
  esp = 2.2e-16;

  // stage parameters
  jsmn::Object& stages = json_config["stages"].unwrap<Object>();
  this->feats.clear();
  this->radius.clear();
  this->probs.clear();
  for (int i = 0; i < T; i++) {
    this->feats.push_back(stages["feature_pool_size"][i].unwrap<Number>());
    this->nps.push_back(stages["neg_pos_ratio"][i].unwrap<Number>());
    this->radius.push_back(stages["random_sample_radius"][i].unwrap<Number>());
    this->probs.push_back(stages["classification_p"][i].unwrap<Number>());
    this->recall.push_back(stages["recall"][i].unwrap<Number>());
    this->drops.push_back(stages["drop_n"][i].unwrap<Number>());
    this->score_normalization_steps.push_back(stages["score_normalization_step"][i].unwrap<Number>());
  }

  // data
  jsmn::Object& data = json_config["data"].unwrap<Object>();
  use_hard = data["use_hard"].unwrap<Boolean>();
  face_txt = data["face"].unwrap<jsmn::String>();
  test_txt = data["test"].unwrap<jsmn::String>();
  jsmn::Array& neg_list = data["background"].unwrap<jsmn::Array>();
  bg_txts.resize(neg_list.size());
  for (int i = 0; i < neg_list.size(); i++) {
    bg_txts[i] = neg_list[i].unwrap<jsmn::String>();
  }

  // status
  jsmn::Object& resume = json_config["resume"].unwrap<jsmn::Object>();
  resume_model = resume["model"].unwrap<jsmn::String>();
  resume_data = resume["data"].unwrap<jsmn::String>();
  snapshot_iter = json_config["snapshot_iter"].unwrap<Number>();

  // fddb benchmark
  jsmn::Object& fddb = json_config["fddb"].unwrap<Object>();
  fddb_dir = fddb["dir"].unwrap<jsmn::String>();
  fddb_result = fddb["out"].unwrap<Boolean>();
  fddb_nms = fddb["nms"].unwrap<Boolean>();
  fddb_minimum_size = fddb["minimum_size"].unwrap<Number>();
  fddb_step = fddb["step"].unwrap<Number>();
  fddb_scale_factor = fddb["scale"].unwrap<Number>();
  fddb_overlap = fddb["overlap"].unwrap<Number>();
  fddb_draw_score = fddb["draw_score"].unwrap<Boolean>();
  fddb_draw_shape = fddb["draw_shape"].unwrap<Boolean>();
  fddb_detect_method = fddb["method"].unwrap<Number>();

  // cart
  jsmn::Object& cart = json_config["cart"].unwrap<Object>();
  restart_on = cart["restart"]["on"].unwrap<Boolean>();
  jsmn::Array& ths = cart["restart"]["th"].unwrap<Array>();
  restart_th.resize(ths.size());
  for (int i = 0; i < restart_th.size(); i++) {
    restart_th[i] = ths[i].unwrap<Number>();
  }
  restart_times = cart["restart"]["times"].unwrap<Number>();

  // face augment
  jsmn::Object& face = json_config["face"].unwrap<Object>();
  face_augment_on = face["online_augment"].unwrap<Boolean>();
  symmetric_landmarks.resize(2);
  int offset = face["symmetric_landmarks"]["offset"].unwrap<Number>();
  jsmn::Array& left = face["symmetric_landmarks"]["left"].unwrap<Array>();
  jsmn::Array& right = face["symmetric_landmarks"]["right"].unwrap<Array>();
  JDA_Assert(left.size() == right.size(), "Symmetric left and right landmarks are not equal size");
  symmetric_landmarks[0].resize(left.size());
  symmetric_landmarks[1].resize(right.size());
  for (int i = 0; i < left.size(); i++) {
    symmetric_landmarks[0][i] = left[i].unwrap<Number>() - offset;
    symmetric_landmarks[1][i] = right[i].unwrap<Number>() - offset;
  }
  with_similarity_transform = face["similarity_transform"].unwrap<Boolean>();

  // pupils
  jsmn::Object& pupils = face["pupils"].unwrap<Object>();
  offset = pupils["offset"].unwrap<Number>();
  jsmn::Array& pupils_left = pupils["left"].unwrap<Array>();
  jsmn::Array& pupils_right = pupils["right"].unwrap<Array>();
  left_pupils.resize(pupils_left.size());
  right_pupils.resize(pupils_right.size());
  for (int i = 0; i < pupils_left.size(); i++) {
    left_pupils[i] = pupils_left[i].unwrap<Number>() - offset;
  }
  for (int i = 0; i < pupils_right.size(); i++) {
    right_pupils[i] = pupils_right[i].unwrap<Number>() - offset;
  }

  // thread_n
  thread_n = omp_get_max_threads();

  // random generator pool
  int rng_size = 2 * thread_n;
  rng_pool.reserve(rng_size);
  for (int i = 0; i < rng_size; i++) {
    rng_pool.push_back(RNG(cv::getTickCount()));
  }
}

Config::~Config() {
}

} // namespace jda
