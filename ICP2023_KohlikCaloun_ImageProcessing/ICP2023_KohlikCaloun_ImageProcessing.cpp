#include <iostream>
#include <opencv2/opencv.hpp>
#include <numeric>

struct image_data {
    cv::Point2f center;
    cv::Mat frame;
};

cv::VideoCapture capture;
bool process_image;
std::mutex image_processing_mutex;
std::unique_ptr<image_data> image_data_ptr;

void image_processing();
cv::Point2f find_center_hsv(cv::Mat& frame, double hue_low, double hue_high, double saturation_low, double saturation_high, double value_low, double value_high);
void draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size);

int main() {
    cv::Mat frame;
    capture = cv::VideoCapture(cv::CAP_DSHOW);

    if (!capture.isOpened()) {
        std::cerr << "Camera didn't open" << std::endl;

        capture = cv::VideoCapture("resources/video.mkv");
        if (!capture.isOpened()) {
            std::cerr << "Video didn't open either" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    process_image = true;
    std::thread thread1(image_processing);

    cv::Point2f center_normalized;
    std::unique_ptr<image_data> image_data_local_ptr;
    while (true) {
        image_processing_mutex.lock();
        if (image_data_ptr) {
            image_data_local_ptr = std::move(image_data_ptr);
        }
        image_processing_mutex.unlock();

        if (image_data_local_ptr) {
            frame = image_data_local_ptr->frame;
            center_normalized = image_data_local_ptr->center;
            draw_cross_normalized(frame, center_normalized, 20);
            cv::namedWindow("Window with your pretty camera");
            cv::imshow("Window with your pretty camera", frame);
            std::cout << "Center relative: " << center_normalized << '\n';
            image_data_local_ptr.reset();
        }
        cv::waitKey(60);

        if (!process_image) {
            break;
        }
    }
    thread1.join();
    std::cout << "End of program" << std::endl;

	return 0;
}

void image_processing() {
    while (true) {
        cv::Mat frame;
        capture.read(frame);
        if (frame.empty()) {
            std::cerr << "Frame is empty, could be because camera closed or video ended" << std::endl;
            break;
        }

        cv::Point2f center_normalized = find_center_hsv(frame, 150.0, 180.0, 100.0, 255.0, 180.0, 255.0);
        image_processing_mutex.lock();
        image_data_ptr = std::make_unique<image_data>();
        image_data_ptr->frame = frame;
        image_data_ptr->center = center_normalized;
        image_processing_mutex.unlock();

        cv::waitKey(1);
    }
    process_image = false;
}

cv::Point2f find_center_hsv(cv::Mat& frame, double hue_low, double hue_high, double saturation_low, double saturation_high, double value_low, double value_high) {
    cv::Mat frame_hsv, frame_treshold;
    cv::cvtColor(frame, frame_hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower_bound(hue_low, saturation_low, value_low);
    cv::Scalar upper_bound(hue_high, saturation_high, value_high);

    cv::inRange(frame_hsv, lower_bound, upper_bound, frame_treshold);

    std::vector<cv::Point> white_pixels;
    cv::findNonZero(frame_treshold, white_pixels);
    cv::Point white_reduced = std::reduce(white_pixels.begin(), white_pixels.end());

    cv::Point2f center_normalized((float)white_reduced.x / white_pixels.size() / frame.cols, (float)white_reduced.y / white_pixels.size() / frame.rows);

    return center_normalized;
}

void draw_cross_normalized(cv::Mat& img, cv::Point2f center_normalized, int size)
{
    center_normalized.x = std::clamp(center_normalized.x, 0.0f, 1.0f);
    center_normalized.y = std::clamp(center_normalized.y, 0.0f, 1.0f);
    int min = img.cols < img.rows ? img.cols : img.rows;
    size = std::clamp(size, 1, min);

    cv::Point2f center_absolute(center_normalized.x * img.cols, center_normalized.y * img.rows);

    cv::Point2f p1(center_absolute.x - size / 2, center_absolute.y);
    cv::Point2f p2(center_absolute.x + size / 2, center_absolute.y);
    cv::Point2f p3(center_absolute.x, center_absolute.y - size / 2);
    cv::Point2f p4(center_absolute.x, center_absolute.y + size / 2);

    cv::line(img, p1, p2, CV_RGB(0, 0, 255), 2);
    cv::line(img, p3, p4, CV_RGB(0, 0, 255), 2);
}