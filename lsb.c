#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int get_image_data_offset(FILE* bmp_offset) {

	fseek(bmp_offset,10,0);
	int offset; 
	offset=(int)fgetc(bmp_offset);
	return offset;

}

int get_message_length(FILE *fp) {

	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	return(size);

}

int get_bit(char the_byte,int which_bit) {

	return((the_byte>>8-which_bit)&1);

}

int main(int argc,char** argv) {

	unsigned char mask_table[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

	FILE *file_handle;
	FILE *message_handle;
	FILE *hidden_message_handle;

	if(argc!=5) {

		printf("Please give correct command line arguments for encoding or decoding :\n\n");
		printf("*** Steganography by LSB substitution***\nUsage: %s [-e][-d] <source file> <destination file> <text file>\n-e : Add text to image\n -d : Get text from image\n",argv[0]);
		exit(1);

	}

	int mode;
	if(!strcmp(argv[1],"-e"))
		mode=1;
	else if(!strcmp(argv[1],"-d"))
		mode=0;
	else {

		printf("Please give correct command line arguments for encoding or decoding :\n\n");
		printf("*** Steganography by LSB substitution***\n\nUsage: %s <mode> <source file> <destination file> <text file>\nMode - e = encrypt \n d= decrypt \n",argv[0]);
		exit(1);

	}

	
	file_handle=fopen(argv[2],"r");

	if (file_handle == NULL) {

		fprintf(stderr, "Can't open input file %s\n",argv[2]);
		exit(1);

	}

	hidden_message_handle=fopen(argv[3],"w");

	if (hidden_message_handle== NULL) {

		fprintf(stderr, "Cannot create output file %s\n",argv[3]);
		exit(1);

	}
	

	int c=0;

	char tmp_sig_cpy;
	int offset=get_image_data_offset(file_handle);

	rewind(file_handle);

	for(int i=0;i<offset;i++) {
		tmp_sig_cpy=fgetc(file_handle);
		fputc(tmp_sig_cpy,hidden_message_handle);
		c++;
	}

	char file_buffer; 			
	char message_buffer;		

	if(mode) {
		printf("\nYou have successfully encoded your secret message.\n\n Please send the image with your hidden message to the relevant person who will be asked for the password.\n");

		message_handle=fopen(argv[4],"r");

		if (message_handle== NULL) {

			fprintf(stderr, "Can't open text input file %s\n",argv[4]);
			exit(1);

		}

		int hidden_message_length=get_message_length(message_handle);

		fputc(hidden_message_length,hidden_message_handle);
		c++;

		do {

			int bit_of_message;
			if(!feof(message_handle)) {	

				message_buffer=fgetc(message_handle);
				for(int i=1;i<=8;i++) {  

					file_buffer=fgetc(file_handle);
					c++;
					int file_byte_lsb = file_buffer & 1; 

					bit_of_message=get_bit(message_buffer,i);
					
					if(file_byte_lsb==bit_of_message) {
						fputc(file_buffer,hidden_message_handle);
					}
					else {
						if(file_byte_lsb==0)

							file_buffer = (file_buffer | 1);

						else

							file_buffer = (file_buffer & ~1);
						
						fputc(file_buffer,hidden_message_handle);
					}
				}
			}

			else {

				tmp_sig_cpy=fgetc(file_handle);
				fputc(tmp_sig_cpy,hidden_message_handle);
				c++;

			}

		} while(!feof(file_handle));

		fclose(message_handle);	
	}

	else {

		char password[] = "teamB24";
		char pass[100];
		int z = 0;
		char c;

		printf("If you are the right person to receive the secret message, then enter the password : \n");
		//scanf("%s",pwd);

		//printf("\n\n\t\t\t\t ENTER PASSWORD: ");
		//printf("\n\n\t\t\t\t  ENTER PASSWORD: ");
        while((c=getch())!=13)
        {
          pass[z++]=c;
          printf("%c",'*');
        }
        pass[z]='\0';

		if(strcmp(password,pass)){
			printf("\n\nYou have entered the wrong password. Please try the decoding process again.\n");
			exit(1);

		}
		printf("Successfully decoded!!\n\n open the file to view the secret message you were supposed to receive.\n");
		message_handle=fopen(argv[4],"w");
		if (message_handle== NULL) {
			fprintf(stderr, "Can't open text input file %s\n",argv[4]);
			exit(1);
		}
	
		int message_length=fgetc(file_handle);
		for(int i=0;i<message_length;i++) {
			char temp_ch='\0';
			for( int j=0;j<8;j++) {
				temp_ch=temp_ch<<1;
				file_buffer=fgetc(file_handle);
				int file_byte_lsb = file_buffer & 1; 
				temp_ch|=file_byte_lsb;
			}
			fputc(temp_ch,message_handle);
		}
		fclose(message_handle);	
	}

	fclose(file_handle);
	fclose(hidden_message_handle);
}