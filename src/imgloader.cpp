// Copyright 2018 chuchienshu
/*
 * Copyright 1993-2007 NVIDIA Corporation.  All rights reserved.
 */


#include <stdio.h>
#include <stdlib.h>
// #include <glog/logging.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef _WIN32
#   pragma warning( disable : 4996 ) // disable deprecated warning 
#endif

#pragma pack(1)


// typedef struct{
//     unsigned char x, y, z, w;// rgb
// } uchar4;

typedef unsigned char uchar;

/*
//
// Read Raw image (8 bit single channel image)
//
extern "C" void LoadRawFile(uchar **dst, int &width, int &height, const char *name)
{
	FILE *fd;

    printf("Loading %s...\n", name);

    if( !(fd = fopen(name,"rb")) ){
        printf("***Raw load error: file access denied***\n");
        exit(0);
    }

	// read size of the image
	fread(&(width), sizeof(int), 1, fd);
	fread(height, sizeof(int), 1, fd);

	*dst = (uchar*)malloc(width * height);

	fread((*dst), sizeof(uchar) * width * height, 1, fd);

	fclose(fd);
}
*/

void LoadIMGFile(uchar4 **dst, int *width, int *height, const char *name)
{
    int x, y;

    // #############
    cv::Mat cv_img;
    cv::Mat recv_img;

    printf("Loading %s...\n", name);

    cv_img = cv::imread(name, CV_LOAD_IMAGE_COLOR);

    // cv::resize(cv_img, recv_img, cv::Size(448, 448));
    if (!cv_img.data){
         printf("img open failed! \n");
         exit(0);

    }

    // *width  = recv_img.cols;;
    // *height = recv_img.rows;

    *width  = cv_img.cols;
    *height = cv_img.rows;

    *dst = (uchar4 *)malloc(*width * *height * sizeof(uchar4)); // why 4 work,but 3 not work?

    printf("img width: %u\n", *width);
    printf("img height: %u\n", *height);
    // printf("img height: %d\n", sizeof(uchar4));

    for(y = 0; y < *height; y++){
        for(x = 0; x < *width; x++){
            (*dst)[(y * *width + x)].z = cv_img.at<cv::Vec3b>(y, x)[2];
            (*dst)[(y * *width + x)].y = cv_img.at<cv::Vec3b>(y, x)[1];
            (*dst)[(y * *width + x)].x = cv_img.at<cv::Vec3b>(y, x)[0];
            // if ((x == 0) && (y == 0)){
            //     printf(" loading down!  --**--");
            //     printf(" %d, %d, %d", (*dst)[(y * *width + x)].z,(*dst)[(y * *width + x)].y,(*dst)[(y * *width + x)].x);
            // }
        }
    }
    // printf(" out %d, %d, %d", (*dst)[(87* 448 + 45)].z,(*dst)[(87* 448 + 45)].y,(*dst)[(87* 448 + 45)].x);

    //创建一个名字为MyWindow的窗口
    // cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
    // //在MyWindow的窗中中显示存储在img中的图片
    // cv::imshow("MyWindow", recv_img);
    // //等待直到有键按下
    // cv::waitKey(0);

}


void LoadGRAYFile(uchar4 **dst, int *width, int *height, const char *name)
{
    int x, y;

    // #############
    cv::Mat cv_img;
    // cv::Mat recv_img;
    cv::Mat gray_img;

    printf("Loading %s...\n", name);

    cv_img = cv::imread(name, CV_LOAD_IMAGE_COLOR);

    // cv::resize(cv_img, recv_img, cv::Size(448, 448));
    if (!cv_img.data){
         printf("img open failed! \n");
         exit(0);

    }

    cvtColor(cv_img, gray_img, CV_RGB2GRAY);//把图片转化为灰度图

    // *width  = recv_img.cols;;
    // *height = recv_img.rows;

    *width  = gray_img.cols;
    *height = gray_img.rows;

    *dst = (uchar4 *)malloc(*width * *height * sizeof(uchar4)); // why 4 work,but 3 not work?

    printf("img width: %u\n", *width);
    printf("img height: %u\n", *height);
    // printf("img height: %d\n", sizeof(uchar4));

    for(y = 0; y < *height; y++){
        for(x = 0; x < *width; x++){
            (*dst)[(y * *width + x)].z = gray_img.at<uchar>(y, x);
            (*dst)[(y * *width + x)].y = gray_img.at<uchar>(y, x);
            (*dst)[(y * *width + x)].x = gray_img.at<uchar>(y, x);
            // if ((x == 0) && (y == 0)){
            //     printf(" loading down!  --**--");
            //     printf(" %d, %d, %d", (*dst)[(y * *width + x)].z,(*dst)[(y * *width + x)].y,(*dst)[(y * *width + x)].x);
            // }
        }
    }
    // printf(" out %d, %d, %d", (*dst)[(87* 448 + 45)].z,(*dst)[(87* 448 + 45)].y,(*dst)[(87* 448 + 45)].x);

    //创建一个名字为MyWindow的窗口
    // cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
    // //在MyWindow的窗中中显示存储在img中的图片
    // cv::imshow("MyWindow", recv_img);
    // //等待直到有键按下
    // cv::waitKey(0);

}

/**
void LoadGRAYFile(uchar4 **dst, int *width, int *height, const char *name)
{
    int x, y;

    // #############
    cv::Mat cv_img;
    cv::Mat recv_img;

    printf("Loading %s...\n", name);

    cv_img = cv::imread(name, CV_LOAD_IMAGE_GRAYSCALE);

    // cv::resize(cv_img, recv_img, cv::Size(448, 448));
    if (!cv_img.data){
         printf("img open failed! \n");
         exit(0);

    }

    // *width  = recv_img.cols;;
    // *height = recv_img.rows;

    *width  = cv_img.cols;
    *height = cv_img.rows;

    *dst = (uchar4 *)malloc(*width * *height * sizeof(uchar4)); // why 4 work,but 3 not work?

    printf("img width: %u\n", *width);
    printf("img height: %u\n", *height);
    // printf("img height: %d\n", sizeof(uchar4));

    for(y = 0; y < *height; y++){
        for(x = 0; x < *width; x++){
            (*dst)[(y * *width + x)].z = cv_img.at<cv::uchar>(y, x);
            (*dst)[(y * *width + x)].y = cv_img.at<cv::uchar>(y, x);
            (*dst)[(y * *width + x)].x = cv_img.at<cv::uchar>(y, x);
            // if ((x == 0) && (y == 0)){
            //     printf(" loading down!  --**--");
            //     printf(" %d, %d, %d", (*dst)[(y * *width + x)].z,(*dst)[(y * *width + x)].y,(*dst)[(y * *width + x)].x);
            // }
        }
    }
    // printf(" out %d, %d, %d", (*dst)[(87* 448 + 45)].z,(*dst)[(87* 448 + 45)].y,(*dst)[(87* 448 + 45)].x);

    //创建一个名字为MyWindow的窗口
    // cv::namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
    // //在MyWindow的窗中中显示存储在img中的图片
    // cv::imshow("MyWindow", recv_img);
    // //等待直到有键按下
    // cv::waitKey(0);

}*/

void SaveIMGFile(uchar4 *dst, int width, int height, const char *outputname){
    // writePPM("debug1.ppm",dst ,*width,*height);

    // unsined char YourData[1024*800];
    cv::Mat saveMat(height, width, CV_8UC4, dst);
    // memcpy(saveMat.data,*dst,448*448*sizeof(uchar4));
    cv::imwrite(outputname, saveMat);

    /**
    FILE* outimage = fopen(outputname, "wb");
    fprintf(outimage, "P6 \n");
    fprintf(outimage, "%d %d \n255\n", *width,*height);

    int x, y;
    for(y = 0; y < *height; y++){
        for(x = 0; x < *width; x++){
            
            // float value1 = valclamp(((*(data+offset0+i))*scale));
            unsigned char aHelp = (*dst)[(y * *width + x)].z;
            fwrite (&aHelp, sizeof(unsigned char), 1, outimage);

            aHelp = (*dst)[(y * *width + x)].y;
            fwrite (&aHelp, sizeof(unsigned char), 1, outimage);
            
            aHelp = (*dst)[(y * *width + x)].x;
            fwrite (&aHelp, sizeof(unsigned char), 1, outimage);

            // if ((x == 0) && (y == 0)){
            //     printf(" loading down!  --**--");
            //     printf(" %d, %d, %d", (*dst)[(y * *width + x)].z,(*dst)[(y * *width + x)].y,(*dst)[(y * *width + x)].x);
            // }
        }
    }

    fclose(outimage);
    */

}
