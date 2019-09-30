#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;

/** @function main */
int main( int argc, char** argv )
{

  Mat src, src_gray, src_blur, path;
  Mat grad, grad2;
  char* window_name = "Sobel Demo - Simple Edge Detector";
  int scale = 1;
  int delta = 0;
  int ddepth = CV_16S;
  int margin = 5;
  double vec[3];


  /// Generate grad_x and grad_y
  Mat grad_x, grad_y;
  Mat abs_grad_x, abs_grad_y;

  /// Load an image
  src = imread( argv[1] );

  if ( !src.data ) return -1;

  /// Create window
  namedWindow( window_name, WINDOW_AUTOSIZE );

  while (src.cols >= margin) {
    GaussianBlur( src, src_blur, Size(3,3), 0, 0, BORDER_DEFAULT );

    /// Convert it to gray
    cvtColor( src_blur, src_gray, COLOR_BGR2GRAY );

    /// Gradient X
    //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_x, abs_grad_x );

    /// Gradient Y
    //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
    Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
    convertScaleAbs( grad_y, abs_grad_y );

    /// Total Gradient (approximate)
    addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
    grad.convertTo(grad2, CV_64FC1, 1.0 / 255.0);

    // std::cout << grad2.type() << " " << grad2.size() << std::endl;

    path = Mat::zeros(grad2.size(), grad2.type());
    grad2.row(0).copyTo(path.row(0));

    for (int i = 1; i < path.rows; i++) {
      for (int j = 0; j < path.cols; j++) {
          if (j == 0) {
              path.at<double>(i, j) = grad2.at<double>(i, j) + std::min({path.at<double>(i - 1, j), path.at<double>(i - 1, j + 1)});
          } else if (j == path.cols - 1) {
              path.at<double>(i, j) = grad2.at<double>(i, j) + std::min({path.at<double>(i - 1, j), path.at<double>(i - 1, j - 1)});
          } else {
              path.at<double>(i, j) = grad2.at<double>(i, j) + std::min({path.at<double>(i - 1, j), path.at<double>(i - 1, j - 1), path.at<double>(i - 1, j + 1)});
          }
      }
    }

    imshow( window_name, grad );
    waitKey(0);

    imshow(window_name, path);
    waitKey(0);

    int row = path.rows - 1;
    cv::Point min_loc;

    minMaxLoc(path.row(row), 0, 0, &min_loc, 0);
    int index = min_loc.x;
    while (row >= 0) {
      if (index == 0) {
        if (path.at<double>(row, 0) > path.at<double>(row, 1)) {
          index = 1;
        } else {
          index = 0;
        }
      } else if (index == path.cols - 1) {
        if (path.at<double>(row, path.cols - 1) > path.at<double>(row, path.cols - 2)) {
            index = path.cols - 2;
        } else {
            index = path.cols - 1;
        }
      } else {
          vec[0] = path.at<double>(row, index - 1);
          vec[1] = path.at<double>(row, index);
          vec[2] = path.at<double>(row, index + 1);
          index = index + (std::min_element(vec, vec + 3) - vec) - 1;
      }

      // std::cout << src.size() << " " << src.row(row).colRange(index, src.cols - margin).size() << " " << src.row(row).colRange(index + margin, src.cols).size() << std::endl;
      // std::cout << row << " " << index << " " << src.cols - margin << std::endl;
      auto x = Rect(index, row, src.cols - (margin + index), 1);
      auto y = Rect(index + margin, row, src.cols - (index + margin), 1);
      // std::cout << x.height << " " << x.width << std::endl;
      // std::cout << y.height << " " << y.width << std::endl;
      // std::cout << src.size() << " " << src.type() << std::endl;

      src(y).copyTo(src(x));
    
      row--;
    }

    src = src(Rect(0, 0, src.cols - margin, src.rows));

    imshow(window_name, src);
    waitKey(0);
  }
}
