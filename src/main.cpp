// Copyright 2009 Matthew Cox and Earl Kinney
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <cuda_runtime.h>
// #include <cutil.h>
// #include <cutil_inline.h>
#include <helper_cuda.h>
#include <helper_cuda_gl.h>
#include <helper_functions.h>
#include <helper_timer.h>
/*
https://bohipat.wordpress.com/2014/07/11/replacing-cutil-in-cuda-5-0/
**/

#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
// #include "bmploader.h"
#include "imgloader.cpp"
#include "nn.h"
#include "nn_gpu.hpp"
// #include <gflags/gflags.h>
#include <fstream>
#include <string>
#include <iostream>

using namespace std;

/* main for reconstruction code */
int main(int argc, char **argv)
{
  if (argc < 1)
  {

    printf("Usage: reconstruct namelist.txt\n");
    exit(1);
  }

  if (NNF_DEBUG)
  {
    printf("A mapent is %i bytes\n", (int)sizeof(mapent));
  }
  // seed random
  srand(time(NULL));
  int fWidth, fHeight, sWidth, sHeight, tWidth, tHeight;
  uchar4 *h_fst, *h_snd, *h_tnd;

  ifstream fin;
  int count = 0;

  fin.open(argv[1]);
  string str, tmp, key_frame, save_name, color_frame;
  mapent *h_map;
  int fPixels;

  tmp = "";
  //     /home/chuchienshu/disk2/eccv_2018/train/input/videvo/
  while (!fin.eof())

  {
    getline(fin, str);
    // cout << str<< endl;

    size_t pos = str.find_last_of('/');
    // key_frame = str.substr(0, pos) + "/00000.jpg";
    // cout << key_frame<< endl;

    // cout << key_frame<< endl;

    // cout<< str.substr(0, pos) << endl;
    if (tmp != str.substr(0, pos) || count % 16 == 0)
    {
      if (tmp != str.substr(0, pos))
      {
        tmp = str.substr(0, pos);
        count = 0;
      }
      count = 0;
      // printf("read new keyframe %c", key_frame);
      // cout<< "read new keyframe " << key_frame << endl;

      // tmp = key_frame;
      key_frame = str.c_str();
      color_frame = str.c_str();
      // color_frame = color_frame.replace(24, 8, "eccv_2018/train/input/videvo");
      // color_frame = color_frame.replace(24, 13, "eccv_2018/test/videvo");

      // // cout << color_frame<< endl;

      // size_t pos1 = color_frame.find("_18600_");
      // color_frame = color_frame.erase(pos1, 7);

      // pos1 = color_frame.find(".png");
      // color_frame = color_frame.replace(pos1, 4, ".jpg");
      cout<< "read new keyframe " << key_frame << endl;
      cout << " key color frame at: " << color_frame << endl;

      LoadGRAYFile(&h_snd, &sWidth, &sHeight, key_frame.c_str());
      LoadIMGFile(&h_tnd, &tWidth, &tHeight, color_frame.c_str());

    }

    count++;
    save_name = str.c_str();
    save_name.insert(pos+1, "PM_");
    int pos1 = save_name.find("videvo");
    // save_name.replace(pos1, 8, "videvo-color-pm");
    save_name.replace(pos1, 6, "videvo-color2gray-pm");

    // pos1 = save_name.find(".png");
    // save_name = save_name.replace(pos1, 4, ".jpg");

    LoadGRAYFile(&h_fst, &fWidth, &fHeight, str.c_str());

    cout<<  "str   " << str << endl;
    cout << "color " << color_frame << endl;
    cout << "key   " << key_frame << endl;
    cout << "save   " << save_name << endl;
    if (count == 16){
        cout << "+++++++++++++++++++++++++++   " << endl;
        exit(0);
    }
    fPixels = fWidth * fHeight;
    //int sPixels = sWidth * sHeight;

    // randomally allocate initial pixel map - size of original image
    // FIXME: what to do for resizing?
   
    h_map = (mapent *)malloc(fPixels * sizeof(mapent));

    nn_random_map(h_fst, fWidth, fHeight, h_snd, sWidth, sHeight, h_map);

    /******************************************************************************/
    // CPU iteration loop

    for (int i = 1; i <= ITERATIONS; i++)
    {

      if (NNF_DEBUG)
      {
        printf("Starting iteration %i...\n", i);
      }

      mapent *newMap;
      newMap = (mapent *)malloc(fPixels * sizeof(mapent));

      // do the search
      nn_search(h_fst, fWidth, fHeight, h_snd, sWidth, sHeight, h_map, newMap);

      // store the results
      free(h_map);
      h_map = newMap;
    }

    nn_viz_map(fWidth, fHeight, h_tnd, sWidth, sHeight, h_map, save_name.c_str());
    free(h_map);
  }

  return 0;
}
