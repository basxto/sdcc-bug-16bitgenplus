#include <stdio.h>

#define UINT8           unsigned char
#define UINT16          unsigned int
#define UINT32          unsigned long int
#define NONBANKED       __nonbanked
#define	J_A             0x10U
#define	J_DOWN          0x08U
#define	J_UP            0x04U
#define specialchar_nl  '\n'
#define specialchar_1   ' '
#define specialchar_2   '>'
#define specialchar_3   'v'

#define buffer_length (16)
UINT8 buffer[buffer_length];
const unsigned char text[] = "not a real dictionary!";

void delay(UINT16 d) NONBANKED {
    return;
}

UINT8 waitpad(UINT8 mask) NONBANKED {
    return 0;
}

UINT8 joypad(void) NONBANKED{
    return 0;
}

void waitpad_any(UINT8 mask){
    return;
}

void space_area(const UINT8 x, const UINT8 y, const UINT8 width, const UINT8 height){
    return;
}

void write_line(UINT8 x, UINT8 y, UINT8 length, char *str) {
    return;
}

UINT8 smart_write(const UINT8 x, const UINT8 y, const UINT8 width, const UINT8 height, char *str){
    UINT8 start = 0;
    UINT8 length;
    UINT8 run = 1;
    UINT8 tmp_y = y;
    UINT8 max = 0;
    UINT8 choices = 0;
    UINT8 firstchoice = y;
    UINT8 jump_back = 0;
    // string return pointer
    char *str_ret = 0;
    // string pointer
    char *str_ptr = str;
    space_area(x, y, width, height);
    while(run){
        // detect choices
        if(start == 0 && *str_ptr == specialchar_2){
            if(choices++ == 0)
                firstchoice = tmp_y;
            buffer[buffer_length - 1] = specialchar_1;
            write_line(x + start, tmp_y, 1, buffer + (buffer_length - 1));
            start += 1;
            str_ptr += 1;
        }
        // regular stuff
        max = 16;
        if(width < start+max)
            max = width-start;
        for(length = 0; length < max; ++length){
            // detect jump command
            if((*str_ptr & 0x80) != 0){
                if(str_ret == 0){
                    // set it to next char
                    str_ret = str_ptr+1;
                }
                UINT8 offset = (*str_ptr & 0x7F)*2;
                // jump to dictionary entry
                str_ptr = text + offset;
                jump_back = 2;
            }
            //get to that next round
            //end of this line
            if(*str_ptr == specialchar_nl || *str_ptr == '\0'){
                buffer[length] = '\0';
                break;
            }
            buffer[length] = *str_ptr;
            ++str_ptr;
            // check if we have to return from dictionary
            if(jump_back != 0){
                --jump_back;
                // actually jump back
                if(jump_back == 0){
                    str_ptr=str_ret;
                    str_ret = 0;
                }
            }
        }

        write_line(x + start, tmp_y, length, buffer);
        start += length;

        if(*str_ptr == '\0'){
            run = 0;
        } else {
            if(*str_ptr == specialchar_nl)
                ++str_ptr;
            start = 0;
            tmp_y += 1;
        }
        if(tmp_y >= y+height){
            // if it reached the width, we overwite the last letter
            if(*(str_ptr-1) != specialchar_nl){
                --str_ptr;
            }
            buffer[0] = specialchar_3;
            buffer[buffer_length - 1] = specialchar_3;
            write_line(x + width - 1, y + height - 1, 1, buffer + (buffer_length - 1));
            delay(100);
            waitpad_any(J_A);
            delay(100);
            tmp_y = y;
            space_area(x, y, width, height);
        }

    }
    // let user select
    if(choices != 0){
        tmp_y = firstchoice;
        run = 1;
        //write arrow
        buffer[buffer_length - 1] = specialchar_2;
        write_line(x, tmp_y, 1, buffer + (buffer_length - 1));
        while(run){
            delay(100);
            buffer[buffer_length - 1] = specialchar_1;
            write_line(x, tmp_y, 1, buffer + (buffer_length - 1));
            switch(joypad()){
                case J_UP:
                    if(tmp_y > firstchoice)
                        --tmp_y;
                    break;
                case J_DOWN:
                    if(tmp_y < firstchoice + choices - 1)
                        ++tmp_y;
                    break;
                case J_A:
                    return (tmp_y - firstchoice) + 1;
                    break;
            }
            buffer[buffer_length - 1] = specialchar_2;
            write_line(x, tmp_y, 1, buffer + (buffer_length - 1));
        }
    }
    return 0;
}

void main(){
    smart_write(0, 0, 20, 20, "test");
}