#include "stego.h"

int main(int argc, char **argv)
{
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    if (!png_ptr || !info_ptr)
    {
        fprintf(stderr, "Failed to create PNG struct.\n");
        return 1;
    }

    png_bytep *row_ptr;

    readPng(argv[2], png_ptr, info_ptr, &row_ptr);
    const size_t height = png_get_image_height(png_ptr, info_ptr);
    const size_t width = png_get_image_width(png_ptr, info_ptr);

    if (strcmp(argv[1], "-enc") == 0)
    {
        char *msg;
        size_t msg_len;

        FILE *f = fopen(argv[3], "r");
        if (f)
        {
            fclose(f);
            msg = readText(argv[3]);
            if (!msg)
            {
                png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
                fprintf(stderr, "Failed to read message from file.\n");
                return 1;
            }
        }
        else
        {
            msg = argv[3];
        }
        msg_len = strlen(msg);

        encodeMsg(row_ptr, msg, msg_len, height, width);

        png_structp png_out_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_out_ptr)
        {
            fprintf(stderr, "Failed to create PNG.\n");
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            if (f) free(msg);
            return 1;
        }

        char *output_file;
        if (argc > 4) {
            output_file = argv[4];
        } else {
            output_file = "out.png";
        }
        writePng(output_file, png_out_ptr, info_ptr, row_ptr);

        printf("Message '%s' encoded !\n", msg);

        if (f) free(msg);
    }
    else if (strcmp(argv[1], "-dec") == 0)
    {
        const size_t height = png_get_image_height(png_ptr, info_ptr);
        char *decoded_msg = NULL;

        Message result = decodeMsg(row_ptr, decoded_msg, width, height);
        decoded_msg = result.msg;

        if (decoded_msg)
        {
            if (argc > 3)
            {
                writeText(argv[3], decoded_msg);
                printf("Message written to: %s\n", argv[3]);
            }
            else printf("Decoded message:\n %s\n", decoded_msg);
        }
        else printf("Failed to decode message\n");
    }
    else
    {
        fprintf(
            stderr, "ERROR: Invalid command\n"
                    "Usage:\n"
                    "Encode message: %s -enc input.png message [output.png]\n"
                    "Decode message: %s -dec input.png\n",
            argv[0], argv[0]
        );
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    return 0;
}