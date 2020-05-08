/*
 * Read and parse the MNIST dataset
 *
 * This is a task that I could accomplish in Python in
 * half the time with half the code.
 *
 * Since I'm trying to learn about C++ optimization here
 * are some areas where this code is suboptimal.  I am
 * not going to solve these problems as this is just
 * example code.
 * In descending order of relevance:
 * 	- Reading from hard disk onto the heap is SUPER
 *		slow.  Reading to stack vars would be
 *		faster;
 * 	- I should be doing some processing while I 
 * 		read from disk; 
 *	- No conception of cache miss performance hit.
 *
 * A Single call to allocate heap space and a single call
 * to free is a good feature
 *
 * TODO:
 * - add labels to the images
 * - add random access
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

#include <assert.h>  // assert


// Swap from little endian to big endian. All ints in file are stored in big-endian (see MNIST)
#define SWAP_INT32(x) (((x) >> 24) | (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8) | ((x) << 24));


int BUF_LEN = 32*sizeof(char);

std::string images = "train-images-idx3-ubyte";
int images_magic = 0x00000803;

struct MNIST {
};

std::string pad(unsigned char pixel, int len) {
	int val = -1*(int)pixel;
	if (val < 0) { val += 256; }
	if (val > 0) { val = 256-val; }
	
	std::stringstream ret;
	ret << std::setfill(' ') << std::setw(len) << val;
	return ret.str();
}


int main() {
	std::ifstream fstream (images);
	if (fstream.is_open()) {
		// allocate memory for reading buffers
		char *buffer = new char[BUF_LEN];
		
		// read int at first position
		fstream.read(buffer, sizeof(int));
		int magic = SWAP_INT32(((int*)buffer)[0]);
		std::cout << "magic:\t  " << magic << "\n";
		assert(magic == images_magic);
		
		fstream.read(buffer, sizeof(int));
		int n_img = SWAP_INT32(((int*)buffer)[0]);
		std::cout << "num img:  " << n_img << "\n";

		// read row and col counts
		fstream.read(buffer, sizeof(int)*2);
		int rows = SWAP_INT32(((int*)buffer)[0]);
		int cols = SWAP_INT32(((int*)buffer)[1]);
		
		std::cout << "rows:\t" << rows << "\n";
		std::cout << "cols:\t" << cols << "\n\n";

		// read each image
		n_img = 10;
		for(int i=0; i<n_img; i++) {
			// create pixel array
			unsigned char img[rows][cols];
			int n_pixels = rows*cols;
			int cur_row = 0;
			int cur_col = 0;
			
			// read chunks of the image (saves reads from the drive)
			while (n_pixels > 0) {
				int read_len;
				if (n_pixels*sizeof(char) > BUF_LEN) {
					read_len = (BUF_LEN/sizeof(char)) * sizeof(char);
				} else {
					read_len = n_pixels*sizeof(char);
				}
				fstream.read(buffer, read_len);
				
				for(int j = 0; j<read_len*sizeof(char); j++) {
					// print
					img[cur_row][cur_col] = *(buffer+j);
					std::cout << pad(img[cur_row][cur_col],4);
					cur_col += 1;
					if (cur_col >= cols) {
						cur_col = 0;
						cur_row += 1;
						std::cout << "\n";
					}
				}
				n_pixels -= read_len;
			}
			// print sep
			std::cout << "\n";
			for (int k=0; k<cols*4; k++) { std::cout << '-'; }
			std::cout << "\n\n";
		}	

		// clean up 
		delete[] buffer;
    		fstream.close();
	}

	return 0;
}
