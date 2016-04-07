/**********************
 * Hello Webcam
 * OpenCV Demo
 * Cooper Bills (csb8@cornell.edu)
 *********************/
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include <opencv/highgui.h> // highgui.h contains openCV gui elements
#include <main.h>
// Main function
int main()
{
    CvCapture* captureDevice; // our webcam handler
    IplImage* currentFrame; // where we'll store the current frame
    captureDevice = cvCaptureFromCAM(1); // get webcam handler.
    if(!captureDevice) {
        printf("Error opening webcam. Exiting. \n");
        return -1;
    }
    // Create a window to display our webcam in
    cvNamedWindow("original", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("modified", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("extMask", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("object1", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("object1_gray", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("trackbars", CV_WINDOW_AUTOSIZE);
    cvCreateTrackbar("H_min", "trackbars", &H_min, 360, NULL);
    cvCreateTrackbar("H_max", "trackbars", &H_max, 360, NULL);
    cvCreateTrackbar("S_min", "trackbars", &S_min, 255, NULL);
    cvCreateTrackbar("S_max", "trackbars", &S_max, 255, NULL);
    cvCreateTrackbar("V_min", "trackbars", &V_min, 255, NULL);
    cvCreateTrackbar("V_max", "trackbars", &V_max, 255, NULL);
    cvCreateTrackbar("HOUGH_PARAM_1", "trackbars", &HOUGH_PARAM_1, 256, NULL);
    cvCreateTrackbar("HOUGH_PARAM_2", "trackbars", &HOUGH_PARAM_2, 256, NULL);

    cvCreateTrackbar("SNG_ERODE_SIZE", "trackbars", &SNG_ERODE_SIZE, 20, NULL);
    cvCreateTrackbar("SNG_MEDIAN_BLUR", "trackbars", &SNG_MEDIAN_BLUR, 21, NULL);
    cvCreateTrackbar("SNG_GAUSSIAN_BLUR", "trackbars", &SNG_GAUSSIAN_BLUR, 21, NULL);
    cvCreateTrackbar("SNG_TRESHOLD", "trackbars", &SNG_TRESHOLD, 256, NULL);
    cvCreateTrackbar("CANNY_PARAM_1", "trackbars", &CANNY_PARAM_1, 256, NULL);
    cvCreateTrackbar("CANNY_PARAM_2", "trackbars", &CANNY_PARAM_2, 256, NULL);
    cvCreateTrackbar("SNG_DILATE_SIZE", "trackbars", &SNG_DILATE_SIZE, 20, NULL);
    cvCreateTrackbar("SNG_PERC_CUTOFF", "trackbars", &SNG_PERC_CUTOFF, 100, NULL);
    cvCreateTrackbar("SNG_DISTORTION_RATIO", "trackbars", &SNG_DISTORTION_RATIO, 10, NULL);
    // Calibrate Camera
    calibrateCamera(captureDevice);
    int key = -1;
    // while there is no key press by the user,
    CvVideoWriter* vidOriWriter;
    CvVideoWriter* vidModWriter;
    char timeStr[60];
    char fname_ori[68];
    char fname_mod[68];
    trackResults trackRes[MAX_NEIGHBOURS + 1];
    while(key == -1) {
        if(WRITE_VIDEO == 1)
        {
            // Write video to file
            cvWriteFrame(vidModWriter,currentFrame);
            currentFrame = cvQueryFrame(captureDevice); // get current frame
            cvFlip(currentFrame,currentFrame,1);
            cvWriteFrame(vidOriWriter,currentFrame);
        }else{
            currentFrame = cvQueryFrame(captureDevice); // get current frame
            cvFlip(currentFrame,currentFrame,1);
        }

        // check for errors:
        if(!currentFrame)
        {
            printf("Current frame not found.\n");
            continue;
        }
        // Display the current frame in the window we created earlier

        trackObjects(currentFrame,&trackRes);
        int i=0;
        CvFont font;
        cvInitFont(&font, CV_FONT_VECTOR0, 0.25, 0.5, 0, 1.5, 8); //rate of width and height is 1:2
        double dist;
        while(trackRes[i].x_p != -1)
        {
            dist = area2distance(trackRes[i].area_p);
            printf("Object %i at distance %f found at (%i,%i)\n", i, dist, trackRes[i].x_p, trackRes[i].y_p);
            cvCircle(currentFrame,cvPoint(trackRes[i].x_p,trackRes[i].y_p),sqrt(trackRes[i].area_p/M_PI),cvScalar(0,255,0,0),1,8,0);
            char str[10];
            sprintf(str,"(%i, %i, %0.2f m)",trackRes[i].x_p,trackRes[i].y_p,dist);
            cvPutText(currentFrame,str,cvPoint(trackRes[i].x_p,trackRes[i].y_p),&font,cvScalar(0,255,0,0));
            i++;
        }
        cvShowImage("original", currentFrame);
        // Wait for user input for 30ms, cvWaitKey return the user's
        // keypress, or -1 if time limit is reached. (0 = wait forever)
        key = cvWaitKey(100);
        switch(key)
        {
            case -1 : break;
            case 114 :
                if(WRITE_VIDEO==1)
                {
                    printf("Stopped recording of files %s_ori.avi and _mod.avi\n",timeStr);
                    cvWriteFrame(vidModWriter,currentFrame);
                    cvReleaseVideoWriter(&vidOriWriter);
                    cvReleaseVideoWriter(&vidModWriter);
                    WRITE_VIDEO = 0;
                }else{

                    time_t rawtime;
                    time(&rawtime);
                    struct tm *info;
                    info = localtime(&rawtime);
                    strftime(timeStr,60,"/home/fred/workspace/recordings/%Y-%m-%d-%H-%M-%S",info);
                    strcpy(fname_ori,timeStr);
                    strcpy(fname_mod,timeStr);
                    strcat(fname_ori,"_ori.avi");
                    strcat(fname_mod,"_mod.avi");
                    printf("Starting recording to files %s_ori.avi and _mod.avi\n",timeStr);
                    vidOriWriter = cvCreateVideoWriter(fname_ori,CV_FOURCC('F','F','V','1'),10,cvSize(currentFrame->width,currentFrame->height),1);
                    vidModWriter = cvCreateVideoWriter(fname_mod,CV_FOURCC('F','F','V','1'),10,cvSize(currentFrame->width,currentFrame->height),1);
                    WRITE_VIDEO = 1;
                }
                key = -1;
                break;
            case 27 : break;
            default:
                printf("Pressed key %i",key); key = -1;
            break;
        }
    }
    // Appropreate Cleanup
    cvReleaseCapture(&captureDevice);
    cvReleaseImage(&currentFrame);
    cvDestroyAllWindows();
    return 0;
}

IplImage* processImage(IplImage* currentFrame)
{
    IplImage* hsvFrame;
    IplImage* tresholdFrame;
    hsvFrame        = cvCreateImage(cvGetSize(currentFrame), 8, 3);
    tresholdFrame   = cvCreateImage(cvGetSize(currentFrame), 8, 1);
    //convert frame from BGR to HSV colorspace
    cvCvtColor(currentFrame, hsvFrame, CV_BGR2HSV);
    if(H_max > 180)
    {   // Adjust for circular Hue value
        IplImage* tresholdFrame2;
        tresholdFrame2   = cvCreateImage(cvGetSize(currentFrame), 8, 1);
        cvInRangeS(hsvFrame,cvScalar(H_min,S_min,V_min, 0),cvScalar(180,S_max,V_max, 0),tresholdFrame);
        cvInRangeS(hsvFrame,cvScalar(0,S_min,V_min, 0),cvScalar(H_max-180,S_max,V_max, 0),tresholdFrame2);
        cvOr(tresholdFrame,tresholdFrame2,tresholdFrame,NULL);
    }else{
        cvInRangeS(hsvFrame,cvScalar(H_min,S_min,V_min, 0),cvScalar(H_max,S_max,V_max, 0),tresholdFrame);
    }
    //Create erode and dialate elements
    IplConvKernel* erodeElm     = cvCreateStructuringElementEx(6,6,3,3,CV_SHAPE_RECT,NULL);
    IplConvKernel* dialateElm   = cvCreateStructuringElementEx(20,20,10,10,CV_SHAPE_RECT,NULL);
    //Erode and dialate treshold
    cvErode(tresholdFrame,tresholdFrame,erodeElm,1);
    cvDilate(tresholdFrame,tresholdFrame,dialateElm,1);
    cvSmooth(tresholdFrame,tresholdFrame,CV_GAUSSIAN,5,0,0,0);
    return tresholdFrame;
}

void trackObjects(IplImage* currentFrame,trackResults* dest)
{
    IplImage* tresholdFrame;
    IplImage* tresholdClone;
    tresholdFrame   = cvCreateImage(cvGetSize(currentFrame), 8, 1);
    tresholdClone   = cvCreateImage(cvGetSize(currentFrame), 8, 1);
    cvSet(tresholdFrame, cvScalar(0,0,0,0),NULL);
    cvSet(tresholdClone, cvScalar(0,0,0,0),NULL);

    tresholdFrame = processImage(currentFrame);
    cvShowImage("modified",tresholdFrame);

    //Create clone for contours
    cvCopy(tresholdFrame,tresholdClone,NULL);
    // Enable/disable lecagy blob fallback
    int legacy_blob_fallback = 0;
    //Track
    CvMemStorage *mem;
    mem = cvCreateMemStorage(0);
    CvSeq *contours = 0;
    int blobs_found = 0;
    int j = 0;
    int circles_found = 0;
    int t = cvFindContours(tresholdClone, mem, &contours, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
    if (t>MAX_NEIGHBOURS)
    {
        printf("Please adjust filter, more than %i blobs found.\n",MAX_NEIGHBOURS);
    }else{
        trackResults tmp_results[t];
        IplImage* maskFrame;
        maskFrame   = cvCreateImage(cvGetSize(tresholdFrame), 8, 1);
        cvSet(maskFrame, cvScalar(0,0,0,0),NULL);
        for(; contours != 0; contours = contours->h_next)
        {
            CvMoments moment;
            cvMoments(contours, &moment, 1);
            double m00 = moment.m00;
            if(m00>ELM_MIN_AREA && m00<ELM_MAX_AREA)
            {
                double m10 = moment.m10;
                double m01 = moment.m01;
                int x = m10/m00;
                int y = m01/m00;
                tmp_results[blobs_found].x_p    = x;
                tmp_results[blobs_found].y_p    = y;
                tmp_results[blobs_found].area_p = m00;
                // Draw rectangle to show cutout for detailed object detection
                cvRectangle(maskFrame,cvPoint(x-sqrt(m00/M_PI),y-sqrt(m00/M_PI)),cvPoint(x+sqrt(m00/M_PI),y+sqrt(m00/M_PI)),cvScalar(255,255,255,0),1,8,0);
                blobs_found++;
            }
        }
        cvShowImage("extMask", maskFrame);
        if(blobs_found>0)
        {
            for(j=0; j<blobs_found; j++)
            {
                // Calculate range & safety factor of found blob to determine size of rectangle
                int obj_r = sqrt(tmp_results[j].area_p/M_PI);
                double safety_f = 1.5;
                // Crop object from currentframe
                cvSetImageROI(currentFrame,cvRect(tmp_results[j].x_p - safety_f*obj_r, tmp_results[j].y_p - safety_f*obj_r, 2*safety_f*obj_r, 2*safety_f*obj_r));
                IplImage* object1_crop  = cvCreateImage(cvGetSize(currentFrame),currentFrame->depth,currentFrame->nChannels);
                cvSet(object1_crop, cvScalar(0,0,0,0),NULL);
                cvCopy(currentFrame,object1_crop,NULL);
                cvResetImageROI(currentFrame);
                IplImage* object1 = cvCreateImage(cvSize(200,200),8,3);
                cvResize(object1_crop,object1,CV_INTER_LINEAR);
                double resizeScale = ((double) object1_crop->width) / object1->width;
                // Allocate other used frames
                //IplImage* object1_hsv   = cvCreateImage(cvGetSize(object1),object1->depth,object1->nChannels);
                IplImage* object1_gray          = cvCreateImage(cvGetSize(object1),object1->depth,1);
                IplImage* object1_gray_contour  = cvCreateImage(cvGetSize(object1),object1->depth,1);

                /* Using simple colour conversion (did not work really well)

                cvCvtColor(object1, object1_gray, CV_BGR2GRAY );
                cvCvtColor(object1, object1, CV_BGR2YCrCb );
                cvCvtColor(object1, object1_hsv, CV_BGR2YCrCb );
                IplImage* object1_Y     = cvCreateImage(cvGetSize(object1),object1->depth,1);
                IplImage* object1_cr    = cvCreateImage(cvGetSize(object1),object1->depth,1);
                IplImage* object1_cb    = cvCreateImage(cvGetSize(object1),object1->depth,1);
                cvSplit(object1,object1_Y,object1_cr,object1_cb,NULL);
                */

                int col, row;
                uchar B, G, R;
                for( row = 0; row < object1->height; row++ )
                {
                    for ( col = 0; col < object1->width; col++ )
                    {
                        B = object1->imageData[object1->widthStep * row + col * 3];
                        G = object1->imageData[object1->widthStep * row + col * 3 + 1];
                        R = object1->imageData[object1->widthStep * row + col * 3 + 2];
                        double gr = R*R/(256) *(R * R  - B * B - G * G) / (R * R);
                        if(gr > 0)
                        {
                            object1_gray->imageData[object1_gray->widthStep * row + col] = round(gr);
                        }else{
                            object1_gray->imageData[object1_gray->widthStep * row + col] = 0;
                        }
                    }
                }
                IplConvKernel* erodeElm = cvCreateStructuringElementEx(SNG_ERODE_SIZE,SNG_ERODE_SIZE,SNG_ERODE_SIZE/2,SNG_ERODE_SIZE/2,CV_SHAPE_RECT,NULL);
                cvErode(object1_gray,object1_gray,erodeElm,1);

                cvSmooth(object1,object1,CV_MEDIAN,floor(SNG_MEDIAN_BLUR/2)*2+1,0,0,0);
                cvSmooth(object1_gray,object1_gray,CV_GAUSSIAN,floor(SNG_GAUSSIAN_BLUR/2)*2+1,0,0,0);

                cvThreshold(object1_gray,object1_gray,SNG_TRESHOLD,255,CV_THRESH_BINARY);
                cvCanny(object1_gray,object1_gray,CANNY_PARAM_1,CANNY_PARAM_2,3);

                IplConvKernel* dilateElm = cvCreateStructuringElementEx(SNG_DILATE_SIZE,SNG_DILATE_SIZE,SNG_DILATE_SIZE/2,SNG_DILATE_SIZE/2,CV_SHAPE_RECT,NULL);
                cvDilate(object1_gray,object1_gray,dilateElm,1);

                /* HSV filtering (did not work really well)

                IplImage* object1_gray_nomask  = cvCreateImage(cvGetSize(object1),object1->depth,1);
                cvCvtColor(object1, object1_hsv, CV_BGR2HSV);
                cvCvtColor(object1, object1_gray_nomask, CV_BGR2GRAY );
                IplImage* object_mask1  = cvCreateImage(cvGetSize(object1),object1->depth,1);
                IplImage* object_mask2  = cvCreateImage(cvGetSize(object1),object1->depth,1);
                cvInRangeS(object1_hsv,cvScalar(160,50,120, 0),cvScalar(180,256,256, 0),object_mask1);
                cvInRangeS(object1_hsv,cvScalar(0,50,120, 0),cvScalar(10,256,256, 0),object_mask2);
                cvOr(object_mask1,object_mask2,object_mask1,NULL);
                //Create erode and dialate elements
                IplConvKernel* erodeElm     = cvCreateStructuringElementEx(8,8,4,4,CV_SHAPE_RECT,NULL);
                IplConvKernel* dialateElm   = cvCreateStructuringElementEx(20,20,10,10,CV_SHAPE_RECT,NULL);
                //Erode and dialate treshold
                //cvErode(object_mask1,object_mask1,erodeElm,1);
                //cvDilate(object_mask1,object_mask1,dialateElm,1);
                //cvSmooth(object_mask1,object_mask1,CV_GAUSSIAN,11,0,0,0);
                cvSet(object1_gray, cvScalar(0,0,0,0),NULL);
                cvCopy(object1_gray_nomask,object1_gray,object_mask1);*/



                // Find cirlce
                if (HOUGH_TRANS == 1)
                {
                    CvMemStorage *mem;
                    mem = cvCreateMemStorage(0);
                    CvSeq* circles = cvHoughCircles(object1_gray,mem,CV_HOUGH_GRADIENT,1.5, object1_gray->width/(2*2*safety_f),HOUGH_PARAM_1,HOUGH_PARAM_2,object1_gray->width/(6*2*safety_f),2*object1_gray->width);
                    if(circles->total > 10 || circles->total == 0)
                    {
                        printf("Please adjust circle filter, %i circles found. Falling back to blob tracking.\n",circles->total);
                        if(legacy_blob_fallback==1)
                        {
                            dest[circles_found] = tmp_results[j];
                            //update circles_found counter
                            circles_found++;
                        }
                    }else{
                        int c;
                        for(c=0;c<circles->total;c++)
                        {
                            float* p = (float*) cvGetSeqElem( circles, c);
                            // Calc area and see if >MIN <MAX
                            double obj_area = M_PI * p[2] * p[2] * resizeScale * resizeScale;
                            if(obj_area>ELM_MIN_AREA && obj_area<ELM_MAX_AREA)
                            {
                                // Calc position of frame & object within frame

                                int frame_x = tmp_results[j].x_p - safety_f*obj_r;
                                int frame_y = tmp_results[j].y_p - safety_f*obj_r;
                                int obj_x =  (frame_x>0)*frame_x + p[0]*resizeScale;
                                int obj_y =  (frame_y>0)*frame_y + p[1]*resizeScale;
                                // Save to dest
                                dest[circles_found].x_p    = obj_x;
                                dest[circles_found].y_p    = obj_y;
                                dest[circles_found].area_p = obj_area;
                                //update circles_found counter
                                if(circles_found==0)
                                {
                                    cvShowImage("object1_gray",object1_gray);
                                    cvShowImage("object1",object1);
                                }
                                circles_found++;
                            }
                        }
                    }
                }else{
                    cvSet(object1_gray_contour, cvScalar(0,0,0,0),NULL);
                    cvCopy(object1_gray,object1_gray_contour,NULL);
                    CvMemStorage *mem;
                    mem = cvCreateMemStorage(0);
                    CvSeq *contours_obj = 0;
                    int t = cvFindContours(object1_gray_contour, mem, &contours_obj, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));
                    if(t>SNG_MAX_NEIGHBOURS || t==0)
                    {
                        switch(t)
                        {
                            case 0  : printf("No contours found. Fallback.\n"); break;
                            default : printf("Too many neighbouring contours found. Fallback.\n"); break;
                        }
                    }else{
                        for(; contours_obj != 0; contours_obj = contours_obj->h_next)
                        {
                            CvMoments obj_moment;
                            cvMoments(contours_obj, &obj_moment, 1);
                            if(obj_moment.m00 > (SNG_PERC_CUTOFF/(float)100) * object1->width * object1->height)
                            {
                                double obj_area = obj_moment.m00 * resizeScale * resizeScale;
                                if(obj_area>ELM_MIN_AREA && obj_area<ELM_MAX_AREA)
                                {
                                    float obj_radius;
                                    CvPoint2D32f obj_centre;
                                    cvMinEnclosingCircle(contours_obj,&obj_centre,&obj_radius);

                                    int frame_x = tmp_results[j].x_p - safety_f*obj_r;
                                    int frame_y = tmp_results[j].y_p - safety_f*obj_r;
                                    int obj_x =  (frame_x>0)*frame_x + obj_centre.x * resizeScale;
                                    int obj_y =  (frame_y>0)*frame_y + obj_centre.y * resizeScale;
                                    float circ_area = M_PI * obj_radius * resizeScale * obj_radius * resizeScale;
                                    if(circ_area < SNG_DISTORTION_RATIO*obj_area)
                                    {
                                        dest[circles_found].x_p    = obj_x;
                                        dest[circles_found].y_p    = obj_y;
                                        dest[circles_found].area_p = circ_area;
                                        //update circles_found counter
                                        if(circles_found==0)
                                        {
                                            cvCircle(object1,cvPoint(obj_centre.x,obj_centre.y),obj_radius,cvScalar(0,0,255,0),1,8,0);
                                            cvShowImage("object1_gray",object1_gray);
                                            cvShowImage("object1",object1);
                                        }
                                        circles_found++;
                                    }else{
                                        //printf("Object with dispr circle rejected: %f > 2*%f\n",circ_area,obj_area);
                                    }
                                }else{
                                    //printf("Object with area %f rejected (sizef: %f pixArea: %f)\n",obj_area,resizeScale,obj_moment.m00);
                                }
                            }else{
                                //printf("Object rejected taking up only %f percent of frame.\n",100*obj_moment.m00/(object1->width*object1->height));
                            }
                        }
                    }
                }
            }
        }
    }
    for(j=MAX_NEIGHBOURS + 1; j>=circles_found; j--)
    {
        dest[j].x_p       = -1;
        dest[j].y_p       = -1;
        dest[j].area_p    = -1;
    }
}
/*
void translate_pb(trackResults* self)
{
    // translate self.x_p / y_p / area_p (pixel coordinate system) to self.x_b / y_b / z_b (body coordinate system)

}
*/
void calibrateCamera(CvCapture* captureDevice)
{
    IplImage* cal_Frame;
    if (access("./cal_1m.jpg",F_OK) != -1)
    {
        printf("Calibration image found.\n");
        cal_Frame = cvLoadImage("./cal_1m.jpg",CV_LOAD_IMAGE_COLOR);
    }else{
        int c_key = -1;
        printf("Please hold marker at 1m distance and press any key.\n");
        trackResults cal_trackRes[MAX_NEIGHBOURS + 1];
        while(c_key == -1)
        {
            cal_Frame = cvQueryFrame(captureDevice);
            trackObjects(cal_Frame,&cal_trackRes);
            if(cal_trackRes[0].x_p != -1)
            {
                cvCircle(cal_Frame,cvPoint(cal_trackRes[0].x_p,cal_trackRes[0].y_p),sqrt(cal_trackRes[0].area_p/M_PI),cvScalar(0,255,0,0),1,8,0);
            }
            cvShowImage("original", cal_Frame);
            c_key = cvWaitKey(10);
        }
        cal_Frame = cvQueryFrame(captureDevice);
        cvSaveImage("./cal_1m.jpg",cal_Frame,0);
    }
    trackResults cal_trackRes[MAX_NEIGHBOURS + 1];
    trackObjects(cal_Frame,&cal_trackRes);
    if(cal_trackRes[0].x_p != -1)
    {
        CAM_FOCAL = (1*sqrt(cal_trackRes[0].area_p/M_PI))/MARKER_RADIUS;
        printf("Calibration ball has area of %f\n",cal_trackRes[0].area_p);
        printf("Determined camera focal length of %f m\n",CAM_FOCAL);
    }else{
        printf("Error: calibration 1m failed.\n");
    }

}

double area2distance(double elmArea)
{
    return MARKER_RADIUS*CAM_FOCAL/sqrt(elmArea/M_PI);
}
