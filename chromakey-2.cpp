/*********************************************************************
 File:     chromakey.cpp

 Author:   Rhea Koparde

 Email address: koparde@usc.edu

 Usage: program_name in.bmp background.bmp dist_threshold out1.bmp out2.bmp

 Notes:
 This program performs the chroma key operation on an input 
 using two different methods.

 Method 1 Utilize a user-defined distance threshold from the
          chromakey value as a discriminator

 Method 2 Devise a method that to determine the chromakey mask
          that doesn't require a user-input threshold

********************************************************************/

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include "bmplib.h"

using namespace std;

// Prototypes
// IMPORTANT: you must exactly use these input types, function names, and 
// return types. Otherwise the grader can't test them.
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold);

void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE]);

void replace(bool mask[][SIZE],
	     unsigned char inImage[][SIZE][RGB],
	     unsigned char bgImage[][SIZE][RGB],
	     unsigned char outImage[][SIZE][RGB]);

int main(int argc, char *argv[])
{
  // Image data array
  // Note:  DON'T use the static keyword except where we expressly say so.
  //        It puts the large array in a separate, fixed, area of memory. 
  //        It is bad practice. But useful until we have dynamic allocation.
  static unsigned char inputImage[SIZE][SIZE][RGB];
  static unsigned char bgrndImage[SIZE][SIZE][RGB];
  static unsigned char outputImage[SIZE][SIZE][RGB];
  static bool chromaMask[SIZE][SIZE];

  double threshold;

  if (argc < 6) {
    cerr << "usage: program_name in.bmp background.bmp dist_threshold " 
         << "out1.bmp out2.bmp" << endl;
    return 0;
  }
	
  if (readRGBBMP(argv[1], inputImage)) {
    cerr << "Error reading file: " << argv[1] << endl;
    return 1;
  }

  if (readRGBBMP(argv[2], bgrndImage)) {
    cout << "Error reading file: " << argv[2] << endl;
    return 1;
  }
  
  // Write code to convert the threshold (argv[3])
  //  from string format to a double and assign the 'threshold'

  threshold = atof(argv[3]);

  // Call Method 1 Function
  method1(inputImage, chromaMask, threshold);

  // Produce the output by calling replace()
  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[4]
  if (writeRGBBMP(argv[4], outputImage)) {
    cout << "Error writing file: " << argv[4] << endl;
    exit(1);
  }	

  // Call Method 2 Function
  method2(inputImage, chromaMask);

  // Produce the output by calling replace()

  replace(chromaMask, inputImage, bgrndImage, outputImage);

  // Write the output image to a file using the filename argv[5]
  if (writeRGBBMP(argv[5], outputImage)) {
    cout << "Error writing file: " << argv[5] << endl;
    exit(1);
  }	

  return 0;
}



// Use user-provided threshold for chroma-key distance
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method1(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE],
	     double threshold)
{
  double redTotal=0; //top left corner
  double greenTotal=0;
  double blueTotal=0;
  int averageCounter=0;
  
  for(int i=0; i<4; i++){
    for(int j=0; j<4; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }

  }


  for(int i=0; i<4; i++){ //top right corner
    for(int j=SIZE-4; j<SIZE; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }


  for(int i=SIZE-4; i<SIZE; i++){ //bottom left corner
    for(int j=0; j<4; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }

  for(int i=SIZE-4; i<SIZE; i++){ //bottom right corner
    for(int j=SIZE-4; j<SIZE; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }

  double redAverage = redTotal/averageCounter;
  double greenAverage = greenTotal/averageCounter;
  double blueAverage = blueTotal/averageCounter;

  for(int i=0; i<SIZE; i++){   //to check if the pixels are a certain distance from chromakey
    for(int j=0; j<SIZE; j++){
      double redDifference = inImage[i][j][0] - redAverage;
      double greenDifference = inImage[i][j][1] - greenAverage;
      double blueDifference = inImage[i][j][2] - blueAverage;
      double totalDistance = sqrt(pow(redDifference, 2)+ 
        pow(greenDifference,2)+ pow(blueDifference,2));

      if(totalDistance < threshold ){
        mask[i][j]=0;
      }
      else{
        mask[i][j]=1;
      }
    }
  }


}





// Devise a method to automatically come up with a threshold
//  for the chroma key determination
// The "output" of this method is to produce a valid mask array
//  where entries in the mask array are 1 for foreground image
//  and 0 for 'background'
void method2(unsigned char inImage[][SIZE][RGB], 
	     bool mask[][SIZE])  
{
  double redTotal=0; //top left corner
  double greenTotal=0;
  double blueTotal=0;

  double redbgrd =0;
  double greenbgrd =0;
  double bluebgrd =0;
  int averageCounter=0;
  int aveForBgrd=0;

  for(int i= 100; i<200; i++){
    for(int j=100; j<200; j++){
      redbgrd += inImage[i][j][0];
      greenbgrd += inImage[i][j][1];
      bluebgrd += inImage[i][j][2];

      aveForBgrd++;
    }
  }



  double redbgrdAverage = redbgrd/aveForBgrd; //averages of the background colors
  double greenbgrdAverage = greenbgrd/aveForBgrd;
  double bluebgrdAverage = bluebgrd/aveForBgrd;


  for(int i=0; i<6; i++){
    for(int j=0; j<6; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }

  }


  for(int i=SIZE-6; i<SIZE; i++){ //top right corner
    for(int j=SIZE-6; j<SIZE; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }


  for(int i=0; i<6; i++){ //bottom left corner
    for(int j=SIZE-6; j<SIZE; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }

  for(int i=SIZE-6; i<SIZE; i++){ //bottom right corner
    for(int j=SIZE-6; j<SIZE; j++){
      redTotal += inImage[i][j][0];
      greenTotal += inImage[i][j][1];
      blueTotal += inImage[i][j][2];

      averageCounter++;
    }
  }

  double redAverage = redTotal/averageCounter; //averages of the colors
  double greenAverage = greenTotal/averageCounter;
  double blueAverage = blueTotal/averageCounter;

  double redthreshold = redbgrdAverage -redAverage;
  double greenthreshold = greenbgrdAverage -greenAverage;
  double bluethreshold = bluebgrdAverage -blueAverage;
  double thresholdDistance= sqrt(pow(redthreshold, 2)+ 
        pow(greenthreshold,2)+ pow(bluethreshold,2));


  for( int i=0; i<SIZE; i++){ 
    for(int j=0; j<SIZE; j++){
        
      double redDifference = inImage[i][j][0] - redAverage;
      double greenDifference = inImage[i][j][1] - greenAverage;
      double blueDifference = inImage[i][j][2] - blueAverage;
      double totalDistance = sqrt(pow(redDifference, 2)+ 
        pow(greenDifference,2)+ pow(blueDifference,2));
      

      if(totalDistance <= thresholdDistance)
      { 
        mask[i][j]=0;
      }
      else{
        mask[i][j]=1;
      }
    }  

  }   
  
}

// If mask[i][j] = 1 use the input image pixel for the output image
// Else if mask[i][j] = 0 use the background image pixel
void replace(bool mask[SIZE][SIZE],
	     unsigned char inImage[SIZE][SIZE][RGB],
	     unsigned char bgImage[SIZE][SIZE][RGB],
	     unsigned char outImage[SIZE][SIZE][RGB])
{
  // Create the output image using the mask to determine
  //  whether to use the pixel from the Input or Background image
  for(int i=0; i<SIZE; i++){
    for(int j=0; j<SIZE; j++){
      if(mask[i][j]==1){
        for(int r=0; r<3; r++){
          outImage[i][j][r]=inImage[i][j][r];
        }
      }
      else{
        for(int r=0; r<3; r++){
          outImage[i][j][r]=bgImage[i][j][r];
        }
      }
    }
  }


}
