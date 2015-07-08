#include <windows.h>
#include <fstream>

using namespace std;

#define	MAX_BUFFER_LENGTH	4096
#define QUIET_ARGUMENT		"-q"
#define MAX_STRING_LENGTH	4097
#define DESTINATION_FILE	"bicplus.txt"
#define LINE_SEPARATOR		"\n"
#define LINE_SEPARATOR_W	"\r\n"
#define TABULATOR			"\t"
#define HEADER_LENGTH		855
#define TAG_CONSTANT		"BI"

bool e=true;

int char_len(const char *s) {
	for(int i = 0; i < MAX_STRING_LENGTH; i ++) {
		if(s[i]==0) return i;
	}
	if(e)printf("An unknown error was found");
	exit(EXIT_FAILURE);
}

bool compare_char(const char *string, const char *mask, int pointer) {
	int l = char_len(mask);
	for(int i = pointer; i < (l + pointer); i ++) {
		if(string[i] != mask[i - pointer]) return false;
	}
	return true;
}

void copy_char(const char *src, int src_start, int src_end, char *dst, int dst_start, int dst_end) {

	int dl=char_len(dst);
	int sl=char_len(src);
	int de=dst_end>dl?dl:dst_end;
	int se=src_end>sl?sl:src_end;
	for(;dst_start<de;dst_start++) {
		if(src_start<se) {
			dst[dst_start]=src[src_start];
		}
		src_start++;
	}
}

int index_of (const char *buf, int buf_len, int buf_off, const char *mask) {
	int mask_len = char_len(mask);
	for (int i = buf_off; i < (buf_len - mask_len + 1); i ++) {
		if(compare_char(buf, mask, i)) {
			return i;
		}
	}
	return -1;
}


int find_char(const char *string, const char *mask, int pointer) {
	int l=char_len(string);
	for(int i=pointer;i<l;i++) {
		if(compare_char(string, mask, i)) return i;
	}
	return -1;
}
/*
void fill_char (char* string, int length, char c) {
	for(int i=0;i<length;i++) {
		string[i] = c;
	}
}
*/
void fill(char* string, int length, const char* chr) {
	for(int i=0;i<length;i++) {
		string[i]=chr[0];
	}
	string[length]=0;
}

char* set_filename(const char* src, const char* name) {
	int pointer=0;
	while(true) {
		int i=find_char(src,"\\",pointer);
		if(i<0) break;
		pointer=i+1;
	}
	//printf("find index for %i\n",pointer);
	int l=char_len(name);
	char* file=new char[pointer+l+1];
	fill(file,pointer+l," ");
	copy_char(src,0,pointer,file,0,pointer);
	copy_char(name,0,l,file,pointer,pointer+l);
	return file;
}

int read_to_buffer(fstream& file, int pointer, char* buffer, int buf_off) {

	if (buf_off > 0) {
		memcpy(buffer, buffer + buf_off, MAX_BUFFER_LENGTH - buf_off);
		memset(buffer + MAX_BUFFER_LENGTH - buf_off, 0, buf_off);
	}
	buf_off = MAX_BUFFER_LENGTH - buf_off;
	//printf("pointer=%i, length=%i, buffer=%s\n", pointer, length, buffer);

	if(!file.is_open()) {
		if(e)printf("Failed to read data. File is not open.");
		exit(EXIT_FAILURE);
	}

	file.seekg(0,ios::end);

	int l = file.tellg();
	l -= pointer;
	if(e)printf("Pointer is %i. Available length is %i\n", pointer, l);

	if(l == 0 && (buffer[0] == 0))return -1;

	file.seekg(pointer);
	if(file.ios::eof()) {
		if(e)printf("The end of file reached if trying set pointer");
		exit(EXIT_FAILURE);
	}
	if(file.ios::bad()) {
		if(e)printf("An unknown error was found if trying set pointer");
		exit(EXIT_FAILURE);
	}

	if (l > MAX_BUFFER_LENGTH - buf_off) {
		l = MAX_BUFFER_LENGTH - buf_off;
	}

	file.read(buffer + buf_off, l);
	if(file.ios::eof()) {
		if(e)printf("The end of file reached");
		exit(EXIT_FAILURE);
	}
	if(file.ios::bad()) {
		if(e)printf("An unknown error was found");
		exit(EXIT_FAILURE);
	}
	//printf("pointer=%i, length=%i, buffer=\"%s\"\n", pointer+length, length, buffer);
	return pointer + l;
}

int max_len_of(const char* str, char delim) {
	int l = char_len(str);
	int max = 0;
	int x = 0;
	for(int i = 0; i < l; i ++) {
		if(str[i] == delim) {
			max = max < x ? x : max;
			x = 0;
		}
		else {
			x ++;
		}
	}
	max = max < x ? x : max;
	return max + 1;
}

int count_of(const char* str, char delim) {
	int l = char_len(str);
	int count = 0;
	int x = 0;
	for(int i = 0; i < l; i ++) {
		if(str[i] == delim) {
			count ++;
			x = 0;
		}
		else {
			x ++;
		}
	}
	return count + 1;
}

char* split(char* str, int count, int max, char delim) {
	char * buf = (char* ) malloc (count * max);
	memset(buf, 0, count * max);
	int counter = 0;
	int pointer = 0;
	int x = 0;
	int l = char_len(str);
	for (int i = pointer; i < l; i ++) {
		if(str[i] == delim) {
			memcpy(buf + counter * max, str + pointer, x);
			pointer = i + 1;
			x = 0;
			if(e) printf("found string%i \"%s\"\n", counter, buf + counter * max);
			counter ++;
		}
		else {
			x ++;
		}
	}
	memcpy(buf + counter * max, str + pointer, x);
	if(e) printf("found string%i '%s'\n", counter, buf + counter * max);
	return buf;
}

void conv_col (const char * col, int max, const char * end, fstream& dst) {
	int l = char_len(col);
	dst.write(col, l > max ? max : l);
	dst.write(end, char_len(end));
}


int convert(fstream& src, int poi, char* buf, int buf_off, fstream& dst) {
	int counter = 0;
	while(true) {

		poi = read_to_buffer(src, poi, buf, buf_off);
		if(poi < 0) {
			if(e)printf("Source file successfully read!\n");
			return counter;
		}
		buf_off = index_of(buf, MAX_BUFFER_LENGTH, 0, LINE_SEPARATOR_W);
		char xxx = buf[buf_off];
		if(buf_off < 0) {
			if(e)printf("Error on end of line (wrong format of source file)\n");
			exit(EXIT_FAILURE);
		}
		buf[buf_off] = 0;
		if(e)printf("pointer=%i, index =%i '%i' buffer=\"%s\"\n", poi, buf_off, xxx, buf);
		int max = max_len_of(buf, TABULATOR[0]);
		int count = count_of(buf, TABULATOR[0]);
		char * line = split(buf, count, max, TABULATOR[0]);
		if(e)printf("line %i has %i columns with max length of %i characters\n", counter, count, max);
		counter ++;

		buf_off += char_len(LINE_SEPARATOR_W);

		conv_col(TAG_CONSTANT, 2, TABULATOR, dst);						//1 - constant BI
		conv_col(line + 0 * max, 1, TABULATOR, dst);					//2 - MODIFICATION FLAG
		conv_col(line + 1 * max, 8, TABULATOR, dst);					//3 - RECORD KEY - truncate to 8 if needed
		conv_col(line + 18 * max, 105, TABULATOR, dst);					//4 - INSTITUTION NAME
		conv_col(line + 25 * max, 35, TABULATOR, dst);					//5 - CITY HEADING from CITY
		conv_col(line + 19 * max, 70, TABULATOR, dst);					//6 - BRANCH INFORMATION
		conv_col(line + 12 * max, 8, TABULATOR, dst);					//7 - BIC CODE from BIC8
		conv_col(line + 13 * max, 3, TABULATOR, dst);					//8 - BRANCH CODE
		conv_col(line + 14 * max, 8, TABULATOR, dst);					//9 - UNIQUE BIC CODE split from BIC
		conv_col(line + 14 * max + 8, 3, TABULATOR, dst);				//10 - UNIQUE BRANCH CODE split from BIC
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//11 - ignore IBAN BIC CODE
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//12 - ignore IBAN BRANCH CODE
		conv_col(line + 0 * max, 0 , TABULATOR, dst);					//13 - ignore ROUTING BIC CODE
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//14 - ignore ROUTING BRANCH CODE
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//15 - ignore PARENT BANK CODE
		conv_col(line + 29 * max, 2, TABULATOR, dst);					//16 - COUNTRY CODE
		conv_col(line + 16 * max, 15, TABULATOR, dst);					//17 - NATIONAL ID
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//18 - ignore UNIQUE NATIONAL ID
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//19 - ignore IBAN COUNTRY CODE
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//20 - ignore IBAN NATIONAL ID
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//21 - ignore UNIQUE IBAN NATIONAL ID
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//22 - ignore OTHER NATIONAL ID 1
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//23 - ignore OTHER NATIONAL ID 2
		conv_col(line + 15 * max, 6, TABULATOR, dst);					//24 - CHIPS UID
		conv_col(line + 31 * max, 4, TABULATOR, dst);					//25 - SUBTYPE INDICATOR
		conv_col(line + 34 * max, 60, TABULATOR, dst);					//26 - SERVICE CODES
		conv_col(line + 33 * max, 35, TABULATOR, dst);					//27 - BRANCH QUALIFIERS
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//28 - ignore SPECIAL CODE
		conv_col(line + 21 * max, 35, TABULATOR, dst);					//29 - PHYSICAL ADDRESS 1 from STREET ADDRESS 1
		conv_col(line + 22 * max, 35, TABULATOR, dst);					//30 - PHYSICAL ADDRESS 2 from STREET ADDRESS 2
		conv_col(line + 23 * max, 35, TABULATOR, dst);					//31 - PHYSICAL ADDRESS 3 from STREET ADDRESS 3
		conv_col(line + 24 * max, 35, TABULATOR, dst);					//32 - PHYSICAL ADDRESS 4 from STREET ADDRESS 4
		conv_col(line + 27 * max, 15, TABULATOR, dst);					//33 - ZIP CODE
		conv_col(line + 26 * max, 90, TABULATOR, dst);					//34 - LOCATION from CPS
		conv_col(line + 28 * max, 70, TABULATOR, dst);					//35 - COUNTRY NAME
		conv_col(line + 20 * max, 35, TABULATOR, dst);					//36 - POB NUMBER
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//37 - ignore POB ZIP CODE
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//38 - ignore POB LOCATION
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//39 - ignore POB COUNTRY NAME
		conv_col(line + 0 * max, 0, TABULATOR, dst);					//40 - ignore NATIONAL ID EXPPIRY DATE
		conv_col(line + 0 * max, 0, LINE_SEPARATOR, dst);				//41 - ignore UPDATE DATE

		free(line);
	}
	exit(EXIT_FAILURE);
}

void conv_head (const char * col, const char * end, fstream& dst) {
	int l = char_len(col);
	dst.write(col, l);
	dst.write(end, char_len(end));
}

void create_header(fstream& dst) {

			conv_head("TAG", TABULATOR, dst);
			conv_head("MODIFICATION FLAG", TABULATOR, dst);
			conv_head("RECORD KEY", TABULATOR, dst);
			conv_head("INSTITUTION NAME", TABULATOR, dst);
			conv_head("CITY HEADING", TABULATOR, dst);
			conv_head("BRANCH INFORMATION", TABULATOR, dst);
			conv_head("BIC CODE", TABULATOR, dst);
			conv_head("BRANCH CODE", TABULATOR, dst);
			conv_head("UNIQUE BIC CODE", TABULATOR, dst);
			conv_head("UNIQUE BRANCH CODE", TABULATOR, dst);
			conv_head("IBAN BIC CODE", TABULATOR, dst);
			conv_head("IBAN BRANCH CODE", TABULATOR, dst);
			conv_head("ROUTING BIC CODE", TABULATOR, dst);
			conv_head("ROUTING BRANCH CODE", TABULATOR, dst);
			conv_head("PARENT BANK CODE", TABULATOR, dst);
			conv_head("COUNTRY CODE", TABULATOR, dst);
			conv_head("NATIONAL ID", TABULATOR, dst);
			conv_head("UNIQUE NATIONAL ID", TABULATOR, dst);
			conv_head("IBAN COUNTRY CODE", TABULATOR, dst);
			conv_head("IBAN NATIONAL ID", TABULATOR, dst);
			conv_head("UNIQUE IBAN NATIONAL ID", TABULATOR, dst);
			conv_head("OTHER NATIONAL ID 1", TABULATOR, dst);
			conv_head("OTHER NATIONAL ID 2", TABULATOR, dst);
			conv_head("CHIPS UID", TABULATOR, dst);
			conv_head("SUBTYPE INDICATOR", TABULATOR, dst);
			conv_head("SERVICE CODES", TABULATOR, dst);
			conv_head("BRANCH QUALIFIER", TABULATOR, dst);
			conv_head("SPECIAL CODE", TABULATOR, dst);
			conv_head("PHYSICAL ADDRESS 1", TABULATOR, dst);
			conv_head("PHYSICAL ADDRESS 2", TABULATOR, dst);
			conv_head("PHYSICAL ADDRESS 3", TABULATOR, dst);
			conv_head("PHYSICAL ADDRESS 4", TABULATOR, dst);
			conv_head("ZIP CODE", TABULATOR, dst);
			conv_head("LOCATION", TABULATOR, dst);
			conv_head("COUNTRY NAME", TABULATOR, dst);
			conv_head("POB NUMBER", TABULATOR, dst);
			conv_head("POB ZIP CODE", TABULATOR, dst);
			conv_head("POB LOCATION", TABULATOR, dst);
			conv_head("POB COUNTRY NAME", TABULATOR, dst);
			conv_head("NATIONAL ID EXPIRY DATE", TABULATOR, dst);
			conv_head("UPDATE DATE", LINE_SEPARATOR, dst);

}

int main(int c, char* arg[]) {

	int poi=0;
	int len;
	char* buf = (char*)malloc(MAX_BUFFER_LENGTH + 1);
	memset(buf, 0, MAX_BUFFER_LENGTH + 1);
	char* dstf;
	char* srcf;
	//read arguments
	/*
	for(int i=0;i<c;i++) {
		printf( "arg %d: %s\n", i, arg[i] );
	}
	*/
	if(c==2) {
		srcf=arg[1];
	}
	else if(c==3) {
		if(compare_char(arg[1],QUIET_ARGUMENT,0) and (char_len(arg[1])==char_len(QUIET_ARGUMENT))) {
			e=false;
		}
		else {
			c=-1;
		}
		srcf=arg[2];
	}
	else {
		c=-1;
	}
	if(c<0) {
		if(e)printf("Please use BicDirConver [-q] source_filename\n");
		if(e)printf("If filename contains a space please envelope the filename with quotes\n");
		if(e)printf("Use -q for quiet working\n");
		exit(EXIT_FAILURE);
	}

	dstf = set_filename(srcf, DESTINATION_FILE);

	//printf("Filename1=%s\n",dst1);
	//printf("Filename2=%s\n",dst2);



	fstream src(srcf, ios::in | ios::binary);
	if(src.is_open()) {
		if(e)printf("Source file %s is open\n", srcf);
	}
	else {
		if(e)printf("file %s is not open", srcf);
		exit(EXIT_FAILURE);
	}


	fstream dst(dstf, ios::out);
	if(dst.is_open()) {
		if(e)printf("Output file HF %s is open\n", dstf);
	}
	else {
		if(e)printf("file %s is not open", dstf);
		exit(EXIT_FAILURE);
	}

	//read header
	src.seekg(poi,ios::end);
	len=src.tellg();
	if(e)printf("Length of %s is %i\n", srcf, len);
	src.seekg(poi);

	poi=read_to_buffer(src, poi, buf, MAX_BUFFER_LENGTH);
	int index = index_of(buf, MAX_BUFFER_LENGTH, 0, LINE_SEPARATOR_W);
	if(index < 0) if(e)printf("Error on end of line (wrong format of source file)\n");
	char xxx = buf[index];
	buf[index] = 0;
	if(e)printf("pointer=%i, index =%i '%i' buffer=\"%s\"\n", poi, index, xxx, buf);

	int max = max_len_of(buf, TABULATOR[0]);
	int count = count_of(buf, TABULATOR[0]);
	char * header = split(buf, count, max, TABULATOR[0]);
	if(e)printf("Header has %i columns with max length of %i characters\n", count, max);
	create_header(dst);
	free(header);
	count = convert(src, poi, buf, index + char_len(LINE_SEPARATOR_W), dst);

	//hs.seekp(0,ios::cur);
	int h=dst.tellp();
	if(e)printf("Output file %s has %i items and %i bytes length\n",dstf,count,h);

	free(buf);
	src.close();
	dst.close();

	return EXIT_SUCCESS;
}
