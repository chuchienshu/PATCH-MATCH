// Copyright 2014 BVLC and contributors.
// This program converts a set of images to a leveldb by storing them as Datum
// proto buffers.
// Usage:
//   convert_imageset [-g] ROOTFOLDER/ LISTFILE DB_NAME RANDOM_SHUFFLE[0 or 1]  [resize_width] [resize_height]
// where ROOTFOLDER is the root folder that holds all the images, and LISTFILE
// should be a list of files as well as their labels, in the format as
//   subfolder1/file1.JPEG 7
//   ....
// if RANDOM_SHUFFLE is 1, a random shuffle will be carried out before we
// process the file lines.
// Optional flag -g indicates the images should be read as
// single-channel grayscale. If omitted, grayscale images will be
// converted to color.

#include <glog/logging.h>
#include <leveldb/db.h>
#include <leveldb/write_batch.h>
#include <lmdb.h>
#include <sys/stat.h>
#include <stdio.h>

#include <algorithm>
#include <fstream>  // NOLINT(readability/streams)
#include <string>
#include <utility>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>

#include "caffe/proto/caffe.pb.h"
#include "caffe/util/io.hpp"
#include "caffe/util/output.hpp"
#include "thirdparty/CImg/CImg.h"
// #include "io.hpp"
// #include "output.hpp"
using namespace cimg_library;

#include <iostream>
using namespace std;

using namespace caffe;  // NOLINT(build/namespaces)
using std::pair;
using std::string;
using std::cout;

class ImagePair
{
public:
    unsigned char* data;
    // unsigned char 用于存储 uint8 图片数据
    int data_size;
    int width;
    int height;

    std::string img1_filename;
    std::string img2_filename;

    ImagePair() { clrptr(); }
    ImagePair(std::string line)
    {
        clrptr();
        stringstream s(line);
        s >> img1_filename >> img2_filename;
    }
    ~ImagePair()
    {
        if(data) delete data;
    }

    void read_data(int w, int h)
    {

        cv::Mat cv_img1;
        cv::Mat cv_img2;
        cv::Mat dst1;
        cv::Mat dst2;


        cv_img1 = cv::imread(img1_filename, CV_LOAD_IMAGE_COLOR);
        cv_img2 = cv::imread(img2_filename, CV_LOAD_IMAGE_COLOR);

        if(w % 32 != 0 || h % 32 != 0){
            LOG(FATAL) << "Width OR height Could not divide by 32 ";
        }

        cv::resize(cv_img1, dst1, cv::Size(w, h));
        cv::resize(cv_img2, dst2, cv::Size(w, h));

        /*
        if (!cv_img1.data) LOG(FATAL) << "Could not open or find file " << img1_filename;
        if (!cv_img2.data) LOG(FATAL) << "Could not open or find file " << img2_filename;*/

        if (!dst1.data) LOG(FATAL) << "Could not open or find file " << img1_filename;
        if (!dst2.data) LOG(FATAL) << "Could not open or find file " << img2_filename;

        int xSize,ySize;

        // xSize=cv_img1.cols;
        // ySize=cv_img1.rows;
        xSize=dst1.cols;
        ySize=dst1.rows;

        assert(dst1.cols==xSize && dst1.rows==ySize );
        assert(dst2.cols==xSize && dst2.rows==ySize );

        width=xSize;
        height=ySize;
        // cout<< "width "<< width << " height " << height << endl;

        data_size=  3*width*height              // Image 1
                   +3*width*height;              // Image 2
        //memset(结构体/数组名 , 用于替换的ASCII码对应字符 , 前n个字符 );
        //memset(结构体/数组名 , "用于替换的字符“ , 前n个字符 );
        data=new unsigned char[data_size]; memset(data,0,data_size);
        unsigned char* ptr=data;

        // Read image 1
        for (int c = 0; c < 3; ++c)
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x) {
                    // unsigned char value=cv_img1.at<cv::Vec3b>(y, x)[c];
                    unsigned char value=dst1.at<cv::Vec3b>(y, x)[c];
                    *(ptr++)=value;
                }
        assert(ptr==data+3*width*height);

        // Read image 2
        for (int c = 0; c < 3; ++c)
            for (int y = 0; y < height; ++y)
                for (int x = 0; x < width; ++x) {
                    // unsigned char value=cv_img2.at<cv::Vec3b>(y, x)[c];
                    unsigned char value=dst2.at<cv::Vec3b>(y, x)[c];
                    *(ptr++)=value;
                }
        assert(ptr==data+2*3*width*height);
        assert(ptr==data+data_size);

    }

    void clear_data()
    {
        delete data;
        clrptr();
    }

protected:
    void clrptr()
    {
        data=0;
        data_size=0;
        width=0;
        height=0;
    }
};

bool EdReadImageToDatum(ImagePair& pair, const int label,
     Datum* datum, int w, int h)
{
  pair.read_data(w, h);
  // 几张图片就设对应的通道数，此处是两张 color image，所以是 6
  datum->set_channels(6);
//   cout<< "datum->set_channels(6) "
//     <<datum->channels() << endl;
  datum->set_height(pair.height);
//   cout<< "datum->set_height(pair.height) "
//     <<pair.height << endl;
  datum->set_width(pair.width);
  datum->set_label(label);
  datum->clear_data();
  datum->set_data(pair.data,pair.data_size);
  pair.clear_data();

  return true;
}

void debugData(string value)
{
    Datum datum;
    datum.ParseFromString(value);

    LOG(INFO) << "channels: " << datum.channels();
    LOG(INFO) << "height: " << datum.height();
    LOG(INFO) << "width: " << datum.width();
    LOG(INFO) << "label: " << datum.label();

    int xSize=datum.width();
    int ySize=datum.height();
    int size=xSize*ySize;
    cout<< xSize << " "
        << ySize << " "
        << size << endl;

    float* image1=new float[xSize*ySize*3];
    float* image2=new float[xSize*ySize*3];

    const unsigned char* ptr=(const unsigned char*)datum.data().c_str();

    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image1[size*0 + y*xSize+x] = *(ptr++);
    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image1[size*1 + y*xSize+x] = *(ptr++);
    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image1[size*2 + y*xSize+x] = *(ptr++);

    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image2[size*0 + y*xSize+x] = *(ptr++);
    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image2[size*1 + y*xSize+x] = *(ptr++);
    for(int y=0; y<ySize; y++)
        for(int x=0; x<xSize; x++)
            image2[size*2 + y*xSize+x] = *(ptr++);
    assert(ptr=(const unsigned char*)(datum.data().c_str()+datum.data().length()));

    writePPM("debug1.ppm",image1,xSize,ySize);
    writePPM("debug2.ppm",image2,xSize,ySize);

    delete image1;
    delete image2;
}

int main(int argc, char** argv) {
  ::google::InitGoogleLogging(argv[0]);
  if (argc < 4 || argc > 9) {
    printf("Convert a set of images to the leveldb format used\n"
        "as input for Caffe.\n"
        "Usage:\n"
        "    convert_imageset_and_flow LISTFILE DB_NAME"
        " RANDOM_SHUFFLE_DATA[0 or 1] DB_BACKEND[leveldb or lmdb]"
        " resize_width  resize_height ");
    return 1;
  }

  int arg_offset = 0;
  
  std::ifstream infile(argv[arg_offset+1]);
  std::vector<ImagePair> lines;
  ImagePair pair;
  std::string line;
  while (getline(infile,line)) {
    lines.push_back(ImagePair(line));
  }
  if (argc >= (arg_offset+4) && argv[arg_offset+3][0] == '1') {
    // randomly shuffle data
    LOG(INFO) << "Shuffling data";
    std::random_shuffle(lines.begin(), lines.end());
  }
  LOG(INFO) << "A total of " << lines.size() << " images.";

  string db_backend = "leveldb";
  if (argc >= (arg_offset+5)) {
    db_backend = string(argv[arg_offset+4]);
    if (!(db_backend == "leveldb") && !(db_backend == "lmdb")) {
      LOG(FATAL) << "Unknown db backend " << db_backend;
    }
  }

  // Open new db
  // lmdb
  MDB_env *mdb_env;
  MDB_dbi mdb_dbi;
  MDB_val mdb_key, mdb_data;
  MDB_txn *mdb_txn;
  // leveldb
  leveldb::DB* db;
  leveldb::Options options;
  options.error_if_exists = true;
  options.create_if_missing = true;
  options.write_buffer_size = 268435456;
  leveldb::WriteBatch* batch;

  // Open db
  LOG(INFO) << "db backend " << db_backend;
  if (db_backend == "leveldb") {  // leveldb
    LOG(INFO) << "Opening leveldb " << argv[arg_offset+2];
    leveldb::Status status = leveldb::DB::Open(
        options, argv[arg_offset+2], &db);
    CHECK(status.ok()) << "Failed to open leveldb " << argv[arg_offset+2];
    batch = new leveldb::WriteBatch();
  } else if (db_backend == "lmdb") {  // lmdb
    LOG(INFO) << "Opening lmdb " << argv[arg_offset+2];
    CHECK_EQ(mkdir(argv[arg_offset+2], 0744), 0)
        << "mkdir " << argv[arg_offset+2] << "failed";
    CHECK_EQ(mdb_env_create(&mdb_env), MDB_SUCCESS) << "mdb_env_create failed";
    CHECK_EQ(mdb_env_set_mapsize(mdb_env, 1099511627776), MDB_SUCCESS)  // 1TB
        << "mdb_env_set_mapsize failed";
    CHECK_EQ(mdb_env_open(mdb_env, argv[arg_offset+2], 0, 0664), MDB_SUCCESS)
        << "mdb_env_open failed";
    CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS)
        << "mdb_txn_begin failed";
    CHECK_EQ(mdb_open(mdb_txn, NULL, 0, &mdb_dbi), MDB_SUCCESS)
        << "mdb_open failed";
  } else {
    LOG(FATAL) << "Unknown db backend " << db_backend;
  }

  // Storing to db
  Datum datum;
  int count = 0;
  const int kMaxKeyLength = 512;
  char key_cstr[kMaxKeyLength];
  int data_size;
  bool data_size_initialized = false;

  for (int line_id = 0; line_id < lines.size(); ++line_id) {
  //for (int line_id = 0; line_id < 1; ++line_id) {
    LOG(INFO) << "adding " << lines[line_id].img1_filename;
    if (!EdReadImageToDatum(lines[line_id], 0, &datum,strtol(argv[5], NULL, 10) ,strtol(argv[6], NULL, 10))) {
      continue;
    }
    if (!data_size_initialized) {
      data_size = datum.channels() * datum.height() * datum.width();
      cout<< data_size << endl;
      data_size_initialized = true;
    }/* else {
      const string& data = datum.data();
      CHECK_EQ(data.size(), data_size) << "Incorrect data field size "
          << data.size();
    }*/
    // sequential
    snprintf(key_cstr, kMaxKeyLength, "%08d_%s", line_id,
        lines[line_id].img2_filename.c_str());
    string value;
    datum.SerializeToString(&value);
    string keystr(key_cstr);
    // debug data
    if(count==0){
       debugData(value);
        // return 0;
    }
    printf(" Len: %d ", value.size());

    // Put in db
    if (db_backend == "leveldb") {  // leveldb
      batch->Put(keystr, value);
    } else if (db_backend == "lmdb") {  // lmdb
      mdb_data.mv_size = value.size();
      mdb_data.mv_data = reinterpret_cast<void*>(&value[0]);
      mdb_key.mv_size = keystr.size();
      mdb_key.mv_data = reinterpret_cast<void*>(&keystr[0]);
      //LOG(INFO) << "mdb put";
      CHECK_EQ(mdb_put(mdb_txn, mdb_dbi, &mdb_key, &mdb_data, 0), MDB_SUCCESS)
          << "mdb_put failed";
    } else {
      LOG(FATAL) << "Unknown db backend " << db_backend;
    }

    if (++count % 100 == 0) {
      // Commit txn
      if (db_backend == "leveldb") {  // leveldb
        db->Write(leveldb::WriteOptions(), batch);
        delete batch;
        batch = new leveldb::WriteBatch();
      } else if (db_backend == "lmdb") {  // lmdb
        CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS)
            << "mdb_txn_commit failed";
        CHECK_EQ(mdb_txn_begin(mdb_env, NULL, 0, &mdb_txn), MDB_SUCCESS)
            << "mdb_txn_begin failed";
      } else {
        LOG(FATAL) << "Unknown db backend " << db_backend;
      }
      LOG(ERROR) << "Processed " << count << " files.";
    }
  }
  // write the last batch
  if (count % 100 != 0) {
    if (db_backend == "leveldb") {  // leveldb
      db->Write(leveldb::WriteOptions(), batch);
      delete batch;
      delete db;
    } else if (db_backend == "lmdb") {  // lmdb
      CHECK_EQ(mdb_txn_commit(mdb_txn), MDB_SUCCESS) << "mdb_txn_commit failed";
      mdb_close(mdb_env, mdb_dbi);
      mdb_env_close(mdb_env);
    } else {
      LOG(FATAL) << "Unknown db backend " << db_backend;
    }
    LOG(ERROR) << "Processed " << count << " files.";
  }
  return 0;
}
