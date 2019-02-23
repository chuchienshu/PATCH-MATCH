
#include <stdio.h>
#include <stdlib.h>
// #include <glog/logging.h>

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

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

int main()
{

    // #############
    cv::Mat cv_img;
    cv::Mat recv_img;

    std::string name = "/home/chuchienshu/disk2/PatchMatch-davis-batch-gray/data/bear.jpg";


    cv_img = cv::imread(name, CV_LOAD_IMAGE_GRAYSCALE);

    // cv::resize(cv_img, recv_img, cv::Size(448, 448));
    if (!cv_img.data){
         printf("img open failed! \n");
         exit(0);

    }

    // *width  = recv_img.cols;;
    // *height = recv_img.rows;
    int width, height;

    width  = cv_img.cols;
    height = cv_img.rows;


    printf("img width: %u\n", width);
    printf("img height: %u\n", height);
    printf("img depth : %u\n", cv_img.channels());
    // printf("img height: %d\n", sizeof(uchar4));
    int temp;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            temp = cv_img.at<uchar>(y, x);
            printf("%d ", temp);
            // (*dst)[(y * *width + x)].y = cv_img.at<cv::Vec3b>(y, x)[1];
            // (*dst)[(y * *width + x)].x = cv_img.at<cv::Vec3b>(y, x)[0];
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

    return 0;

}