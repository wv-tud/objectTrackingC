#ifndef MAIN_H
#define MAIN_H
typedef struct trackResults {
    int     x_p;
    int     y_p;
    double  area_p;
    int     x_b;
    int     y_b;
    int     z_b;
    int     x_w;
    int     y_w;
    int     z_w;
} trackResults;
// Define functions
IplImage* processImage(IplImage*);
void trackObjects(IplImage*,trackResults[]);
//void translate_pb(trackResults*);
void calibrateCamera(CvCapture*);
double area2distance(double);
// Environment variables
int H_min           = 175;
int H_max           = 189;
//int S_min           = 120;
int S_min           = 80;
int S_max           = 256;
int V_min           = 127;
int V_max           = 256;
int HOUGH_PARAM_1   = 80;
int HOUGH_PARAM_2   = 100;
int MAX_NEIGHBOURS  = 20;
double ELM_MIN_AREA = 25;
double ELM_MAX_AREA = 50000;
double CAM_FOCAL    = 0;
double CAM_KNOWN_W  = 0;
double MARKER_RADIUS= 0.05;
double MIN_PIXEL_RADIUS = 6;
double MAX_PIXEL_RADIUS = 1000;
int WRITE_VIDEO = 0;
int HOUGH_TRANS = 0;
// Single object detection parameters
int SNG_MAX_NEIGHBOURS  = 5;
int SNG_ERODE_SIZE      = 4;
int SNG_MEDIAN_BLUR     = 5;
int SNG_GAUSSIAN_BLUR   = 5;
//int SNG_TRESHOLD        = 75;
int SNG_TRESHOLD        = 20;
int CANNY_PARAM_1       = 80;
int CANNY_PARAM_2       = 100;
int SNG_DILATE_SIZE     = 3;
int SNG_PERC_CUTOFF     = 3;
int SNG_DISTORTION_RATIO = 4;
#endif // MAIN_H

