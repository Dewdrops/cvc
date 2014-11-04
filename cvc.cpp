/*
 * cvc.cpp
 * Copyright (C) 2014 dewdrops <dewdrops@rdcps>
 *
 * Distributed under terms of the MIT license.
 */

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

static char output_filename[1024];
static char input_filename[1024];

static int frame_range[2] = {0, 0};

static bool flag_extract    = false,
            flag_gray       = false,
            flag_outputfile = false;

void extract_range(const char *range_str)
{
    frame_range[0] = atoi(range_str);

    const char *p = strchr(range_str, '-');
    if (p != NULL) {
        p += 1;
        frame_range[1] = atoi(p);
    }
}

void subVideo(VideoCapture &capture, int beg, int end)
{
    VideoWriter writer;
    int input_ex = static_cast<int>(capture.get(CV_CAP_PROP_FOURCC));
    Size input_size = Size((int) capture.get(CV_CAP_PROP_FRAME_WIDTH),
                (int) capture.get(CV_CAP_PROP_FRAME_HEIGHT));
    double input_fps = capture.get(CV_CAP_PROP_FPS);

    Mat frame, frame_gray;
    int i;
    for (i = 0; i < frame_range[0]-1; ++i) {
        capture >> frame;
    }

    writer.open(output_filename, input_ex, input_fps, input_size);
    if (! writer.isOpened()) {
        printf("Load file %s failed\n", output_filename);
        exit(-1);
    }

    for ( ; i <= frame_range[1]; ++i) {
        capture >> frame;
        if (flag_gray) {
            cvtColor(frame, frame_gray, CV_RGB2GRAY);
            writer << frame;
        }
        else {
            writer << frame;
        }
    }
}

int main(int argc, char *argv[])
{
    const char *optstring = "o:ge:";
    option long_options[] = {
        {"output",  required_argument, NULL, 'o'},
        {"gray",    no_argument,       NULL, 'g'},
        {"extract", required_argument, NULL, 'e'},
        {0, 0, 0, 0}
    };

    char opt;
    while ((opt = getopt_long(argc, argv,
                        optstring, long_options, NULL)) != -1) {
        switch (opt) {
            case 'o':
                flag_outputfile = true;
                strncpy(output_filename, optarg, 1024);
                break;
            case 'e':
                flag_extract = true;
                extract_range(optarg);
                break;
            case 'g':
                flag_gray = true;
                break;
            default:
                exit(-1);
        }
    }

    if (!flag_outputfile || argv[optind] == NULL) {
        printf("Usage: cvc [options] filename\n");
        exit(-1);
    }
    strncpy(input_filename, argv[optind], 1024);

    VideoCapture capture;
    capture.open(input_filename);
    if (! capture.isOpened()) {
        printf("Load file %s failed\n", input_filename);
        exit(-1);
    }
    int input_frame_cnt = capture.get(CV_CAP_PROP_FRAME_COUNT);

    if (flag_extract) {
        if (frame_range[1] == 0) {
            Mat frame, frame_gray;

            if (frame_range[0] < 1 || frame_range[0] > input_frame_cnt) {
                printf("Invalid frame id");
                exit(-1);
            }
            for (int i = 0; i < frame_range[0]; ++i) {
                capture >> frame;
            }

            if (flag_gray) {
                cvtColor(frame, frame_gray, CV_RGB2GRAY);
                imwrite(output_filename, frame_gray);
            }
            else {
                imwrite(output_filename, frame);
            }
        }
        else {
            if (frame_range[0] < 1 || frame_range[0] > input_frame_cnt ||
                        frame_range[1] < 1 || frame_range[1] > input_frame_cnt ||
                        frame_range[0] > frame_range[1]) {
                printf("Invalid frame id");
                exit(-1);
            }

            subVideo(capture, frame_range[0], frame_range[1]);
        }
    }
    else if (flag_gray) {
        subVideo(capture, 1, input_frame_cnt);
    }
    else {
        printf("No operation specified");
        exit(-1);
    }

    return 0;
}

