COSS_COMPLIER=arm-none-linux-gnueabi-
CC=$(COSS_COMPLIER)gcc
OBJS=main.o speak_text.o
LIBPATH=-lpthread -luart -lv4l2 -ljpeg -L ./lib_arm/

main:$(OBJS)
	$(CC) $^ -lpthread -o $@ $(LIBPATH)

%.o:%.c
	$(CC) -I ./include_arm/ -c $< -o $@

clean:
	rm main *.o 




