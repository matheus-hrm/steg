#include "stego.h"

char *readText(const char *file_path)
{
    FILE *file = fopen(file_path, "r");
    if (!file)
    {
        fprintf(stderr, "Error opening file %s.\n", file_path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    rewind(file);

    char *file_content = (char *)malloc((file_size + 1) * sizeof(char));
    if (!file_content)
    {
        fprintf(stderr, "Error allocating memory for file content.\n");
        fclose(file);
        return NULL;
    }

    fread(file_content, sizeof(char), file_size, file);
    file_content[file_size] = '\0';

    fclose(file);
    return file_content;
}

void writeText(const char *file_path, const char *content)
{
    FILE *file = fopen(file_path, "w");
    if (!file)
    {
        fprintf(stderr, "Error opening output file.\n");
        return;
    }

    fprintf(file, "%s", content);
    fclose(file);
}

void readPng(char *file_name, png_structp png_ptr, png_infop info_ptr, png_bytep **row_ptr)
{
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
    {
        fprintf(stderr, "Failed to open %s for reading.\n", file_name);
        abort();
    }

    png_init_io(png_ptr, fp);
    png_read_info(png_ptr, info_ptr);

    png_byte color_type = png_get_color_type(png_ptr, info_ptr);
    if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
        png_set_palette_to_rgb(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
    }

    const size_t height = png_get_image_height(png_ptr, info_ptr);
    *row_ptr = (png_bytep *)malloc(sizeof(png_bytep) * height);
    if (!*row_ptr)
    {
        fprintf(stderr, "Failed to allocate memory for row pointers.\n");
        abort();
    }

    for (int y = 0; y < height; y++)
    {
        (*row_ptr)[y] = (png_byte *)malloc(png_get_rowbytes(png_ptr, info_ptr)); // memoria para cada linha
        if (!(*row_ptr)[y])
        {
            fprintf(stderr, "Failed to allocate memory for row %d.\n", y);
            abort();
        }
    }
    png_read_image(png_ptr, *row_ptr);
    fclose(fp);
}

void writePng(char *file_name, png_structp png_ptr, png_infop info_ptr, png_bytep *row_ptr)
{
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
    {
        fprintf(stderr, "Failed to open %s for writing.\n", file_name);
        abort();
    }

    png_init_io(png_ptr, fp);
    png_write_info(png_ptr, info_ptr);
    png_write_image(png_ptr, row_ptr);
    png_write_end(png_ptr, NULL);

    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (fp != NULL)
        fclose(fp);
}

void encodeMsg(png_bytep *byte_arr, char *msg, size_t msg_len, size_t width, size_t height)
{
    if (msg_len > (height * width - 32)) // 32 bits para o tamanho da mensagem
    {
        fprintf(stderr, "Message too large to encode in image.\n");
        abort();
    }
    int i = 0, j = 0, num_bits_msg_len = 32;
    // ========================
    // implementacao do algoritmo LSB
    // byte_arr[i][j] & 0xFE zera o bit menos significativo
    // (msg_len >> num_bits_msg_len) & 0x01 pega o bit que queremos codificar para o bit menos significativo, 
    // o OR combina o byte da imagem com o LSB zerado com o bit da mensagem. 
    // i.e. insere o bit da mensagem no byte da imagem
    // ========================
    while (num_bits_msg_len--)
    {
        byte_arr[i][j] = (byte_arr[i][j] & 0xFE) | ((msg_len >> num_bits_msg_len) & 0x01);
        j++;
        if (j == width)
        {
            i++;
            j = 0;
        }
    }
    
    size_t num_bits_msg = msg_len * 8;
    // (msg[num_bits_msg / 8] >> (num_bits_msg % 8)) & 0x01: Isso pega o bit atual da mensagem que estamos codificando. msg[num_bits_msg / 8] seleciona o byte correto da mensagem 
    //(num_bits_msg % 8) seleciona o bit correto dentro desse byte. Fazendo uma operação AND com 0x01, nós mantemos apenas o bit menos significativo.
    while (num_bits_msg--)
    {
        byte_arr[i][j] = (byte_arr[i][j] & 0xFE) | ((msg[num_bits_msg / 8] >> (num_bits_msg % 8)) & 0x01);
        j++;
        if (j == width)
        {
            i++;
            j = 0;
        }
    }
}

Message decodeMsg(png_bytep *byte_arr, char *msg, size_t width, size_t height)
{
    int msg_len = 0;
    int i = 0, j = 0, num_bits_msg_len = 32;
    while (num_bits_msg_len--)
    {
        msg_len = (msg_len << 1) | (byte_arr[i][j] & 0x01);
        j++;
        if (j == width)
        {
            i++;
            j = 0;
        }
    }

    msg = (char *)malloc(sizeof(char) * (msg_len + 1));
    if (!msg)
    {
        fprintf(stderr, "Failed to allocate memory for message.\n");
        abort();
    }
    msg[msg_len] = '\0';

    size_t num_bits_msg = msg_len * 8;
    while (num_bits_msg--)
    {
        msg[num_bits_msg / 8] = (msg[num_bits_msg / 8] << 1) | (byte_arr[i][j] & 0x01);
        j++;
        if (j == width)
        {
            i++;
            j = 0;
        }
    }

    Message result = {msg, msg_len};
    return result;
}
