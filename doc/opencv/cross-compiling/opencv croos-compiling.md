Your compiling tools are installed, we will now use them to (cross-)compile opencv for the RPi.

Here the important infos come from this link : http://docs.opencv.org/3.1.0/d0/d76/tutorial_arm_crosscompile_with_cmake.html

One can follow them easily to download opencv source code. Then to prepare the cmake config files, you can use the following command :

  cmake -DCMAKE_TOOLCHAIN_FILE=~/raspberry/pi.cmake ../../..

It tells cmake to use the custom cross compiler (use version 4.9 to avoid further trouble !) to compile opencv
Then, before compiling it you need to configure the opencv variable to disable modules that don't compile.
[This section is inspired from the following link : https://aplacetogeek.wordpress.com/qt-opencv-v4l-cross-compiling-raspberry-pi-beagle-bone/]
To do this you can use cmake-gui (that you can download using apt-get install).
In GUI you need to tick the "Advanced" box, and then to disable these things :

  BUILD_NEW_PYTHON_SUPPORT
  BUILD_TESTS
  WITH_1394
  WITH_CUDA
  WITH_EIGEN2 (and WITH_EIGEN)
  WITH_FFMPEG
  WITH_GSTREAMER
  WITH_GTK
  WITH_JASPER
  WITH_JPEG
  WITH_OPENEXR
  WITH_PNG
  WITH_PVAPI
  WITH_QT
  WITH_QT_OPENGL
  WITH_TBB
  WITH_TIFF
  WITH_UNICAP
  WITH_V4L
  WITH_XINE

The press "Configure" and finally "Generate" to create the correct cmake configuration files.
And then you can compile opencv for the RPi using :

make -j 4

(The "-j 4" option tells make to use 4 threads. Adjust it to the number of threads your processor can handle, it will speed up the compilation)

Now that opencv is compiled, you need to "install" it using

make install

This command puts all files that need to be copied on the RPi in the opencv/platforms/linux/build_hardfp

You can finally copy these files using rsync (again) :

rsync -rl --delete-after --safe-links install/* pi@$PI:/home/pi/opencv

We first copy the files in the /home/pi/opencv folder, because the pi user doesn't have the permissions to edit the /usr/local folder (where we will put the newly compiled libraries)
Hence on the pi we need to copy these files to the correct location:

sudo cp -R /home/pi/opencv/* /usr/local/

That's it! You (normally) successfully cross-compiled opencv for your Raspberry Pi, and you can now use it to do great things!
