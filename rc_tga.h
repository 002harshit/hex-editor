/*
 * TGA File Handling Library
 * by Michael Binder (RednibCoding)
 *
 * This library provides functionality to load and save TGA (Targa) image files, including support for uncompressed
 * and RLE-compressed (32-bit RGBA and 24-bit RGB) images. The library is simple and self-contained, designed for
 * easy integration into projects requiring TGA image manipulation.
 *
 * Key Features:
 * - TGA Loading from Memory and Files:
 *   - rc_load_tga_mem: Loads a TGA image directly from a memory buffer.
 *   - rc_load_tga: Loads a TGA image from a file on disk.
 *
 * - TGA Writing to Memory and Files:
 *   - rc_write_tga_mem: Converts raw pixel data to a TGA format and stores it in a memory buffer.
 *   - rc_write_tga: Saves raw pixel data as a TGA file to disk.
 *   - rc_write_tga_as_c_array: writes the loaded TGA file as c array (.h file) to disk.
 *
 *
 * Usage:
 * This library can be used in projects that need to handle TGA files for image loading or manipulation,
 * particularly in contexts such as game development or graphics tools.
 *
 * Define RC_TGA_IMPL before including rc_tga.h in *one* c-file to create the implementations. All other files can just include rc_tga.h.
 *
 * LICENSE: public domain ⁠— no warranty implied; use at your own risk.
 */

#ifndef _RC_TGA_H_
#define _RC_TGA_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char *rc_load_tga_mem(unsigned char *data, size_t size, int *width, int *height);
unsigned char *rc_load_tga(const char *filename, int *width, int *height);

#ifdef __cplusplus
}
#endif

#endif // _RC_TGA_H_

#ifdef RC_TGA_IMPL

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned char *rc_load_tga_mem(unsigned char *data, size_t size, int *width, int *height)
{
	if (size < 18) {
		fprintf(stderr, "Invalid TGA data: insufficient size\n");
		return NULL;
	}

	// Read the TGA header
	unsigned char *header = data;

	// Verify image type (uncompressed or RLE compressed true-color)
	if (header[2] != 2 && header[2] != 10) {
		fprintf(stderr, "Unsupported TGA image type (only uncompressed or RLE true-color supported)\n");
		return NULL;
	}

	// Extract image dimensions and pixel depth
	*width = header[12] | (header[13] << 8);
	*height = header[14] | (header[15] << 8);
	int pixel_depth = header[16];

	if (pixel_depth != 32 && pixel_depth != 24) {
		fprintf(stderr, "Unsupported TGA pixel depth (only 24-bit and 32-bit supported)\n");
		return NULL;
	}

	const unsigned char *pixel_data = data + 18;
	size_t data_size = size - 18;

	unsigned char *pixels = malloc((*width) * (*height) * 4);
	if (!pixels) {
		fprintf(stderr, "Memory allocation failed for pixel data\n");
		return NULL;
	}

	if (header[2] == 2) {
		// Uncompressed TGA
		size_t bytes_per_pixel = pixel_depth / 8;
		if (data_size < (*width) * (*height) * bytes_per_pixel) {
			fprintf(stderr, "Insufficient pixel data for uncompressed TGA\n");
			free(pixels);
			return NULL;
		}

		for (int i = 0; i < (*width) * (*height); i++) {
			int src_index = i * bytes_per_pixel;
			int dst_index = i * 4;

			pixels[dst_index + 0] = pixel_data[src_index + 0];								  // Blue
			pixels[dst_index + 1] = pixel_data[src_index + 1];								  // Green
			pixels[dst_index + 2] = pixel_data[src_index + 2];								  // Red
			pixels[dst_index + 3] = (bytes_per_pixel == 4) ? pixel_data[src_index + 3] : 255; // Alpha
		}
	} else if (header[2] == 10) {
		// RLE-compressed TGA
		const unsigned char *ptr = pixel_data;
		unsigned char *out_ptr = pixels;
		int pixel_count = (*width) * (*height);
		int bytes_per_pixel = pixel_depth / 8;

		while (pixel_count > 0 && ptr < data + size) {
			unsigned char packet_header = *ptr++;
			int packet_size = (packet_header & 0x7F) + 1;

			if (packet_header & 0x80) {
				// RLE packet
				if (ptr + bytes_per_pixel > data + size) {
					fprintf(stderr, "Insufficient RLE pixel data\n");
					free(pixels);
					return NULL;
				}

				for (int i = 0; i < packet_size; i++) {
					if (pixel_count <= 0)
						break;

					out_ptr[0] = ptr[0];								// Blue
					out_ptr[1] = ptr[1];								// Green
					out_ptr[2] = ptr[2];								// Red
					out_ptr[3] = (bytes_per_pixel == 4) ? ptr[3] : 255; // Alpha

					out_ptr += 4;
					pixel_count--;
				}
				ptr += bytes_per_pixel;
			} else {
				// Raw packet
				int raw_size = packet_size * bytes_per_pixel;
				if (ptr + raw_size > data + size) {
					fprintf(stderr, "Insufficient raw pixel data\n");
					free(pixels);
					return NULL;
				}

				for (int i = 0; i < packet_size; i++) {
					if (pixel_count <= 0)
						break;

					out_ptr[0] = ptr[0];								// Blue
					out_ptr[1] = ptr[1];								// Green
					out_ptr[2] = ptr[2];								// Red
					out_ptr[3] = (bytes_per_pixel == 4) ? ptr[3] : 255; // Alpha

					out_ptr += 4;
					ptr += bytes_per_pixel;
					pixel_count--;
				}
			}
		}

		if (pixel_count > 0) {
			fprintf(stderr, "Not enough pixel data for RLE-compressed TGA\n");
			free(pixels);
			return NULL;
		}
	}

	return pixels;
}

unsigned char *rc_load_tga(const char *filename, int *width, int *height)
{
	FILE *file = fopen(filename, "rb");
	if (!file) {
		perror("Failed to open TGA file");
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long file_size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (file_size <= 0) {
		fprintf(stderr, "Invalid TGA file size\n");
		fclose(file);
		return NULL;
	}

	unsigned char *data = malloc(file_size);
	if (!data) {
		fprintf(stderr, "Memory allocation failed for file data\n");
		fclose(file);
		return NULL;
	}

	if (fread(data, 1, file_size, file) != (size_t)file_size) {
		fprintf(stderr, "Failed to read TGA file\n");
		free(data);
		fclose(file);
		return NULL;
	}

	fclose(file);

	unsigned char *pixels = rc_load_tga_mem(data, file_size, width, height);
	free(data);
	return pixels;
}

#ifdef __cplusplus
}
#endif

#endif // RC_TGA_IMPL
