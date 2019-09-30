# Image Carving

This repository implements seam carving algorithms in Python and C++. Inspired by an early CV image carving paper, this algorithm uses a simple DP shortest path algorithm weighted by local image gradients.

## Usage

Run the Python implementation with `python carving.py [path to image file] -m [pixels per cut]`. Repeatedly hitting enter will carving the specified number of pixels from the scene.

To run the C++ version, first locate your installation of OpenCV and update the Makefile to use this file. Then run `make` and `./carve [path to file]`. The C++ version has a bug in copyTo for small images that I can't track down.