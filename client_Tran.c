// Peter Tran A11163016
#include <stdio.h>
#include <string.h>
 
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
// ECE158A Project
// Name: Peter Tran
// PID: A11163016

/********************************** Upload stuff ********************************/

int upload(void){
  CURL *curl;
  CURLcode res;
  struct stat file_info;
  double speed_upload, total_time;
  FILE *fd;
 
  fd = fopen("upload_test.txt", "rb"); /* open file to upload */ 
  if(!fd) {
 
    return 1; /* can't continue */ 
  }
 
  /* to get the file size */ 
  if(fstat(fileno(fd), &file_info) != 0) {
 
    return 1; /* can't continue */ 
  }
 
  curl = curl_easy_init();
  if(curl) {
    /* upload to this place */ 
    curl_easy_setopt(curl, CURLOPT_URL,
                     "ftp://ece158a:aloha15markov@52.34.178.188/upload_Tran.txt");
 
    /* tell it to "upload" to the URL */ 
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
 
    /* set where to read from (on Windows you need to use READFUNCTION too) */ 
    curl_easy_setopt(curl, CURLOPT_READDATA, fd);
 
    /* and give the size of the upload (optional) */ 
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
                     (curl_off_t)file_info.st_size);
 
    /* enable verbose for easier tracing */ 
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    }
    else {
      /* now extract transfer info */ 
      curl_easy_getinfo(curl, CURLINFO_SPEED_UPLOAD, &speed_upload);
      curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);
 
      fprintf(stderr, "Speed: %.3f bytes/sec during %.3f seconds\n",
              speed_upload, total_time);
 
    }
    /* always cleanup */ 
    curl_easy_cleanup(curl);
  }
  return 0;
}
/*********************************************************************************/

/********************************** Download stuff *******************************/
struct FtpFile {
  const char *filename;
  FILE *stream;
};
 
static size_t my_fwrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
  struct FtpFile *out=(struct FtpFile *)stream;
  if(out && !out->stream) {
    /* open file for writing */ 
    out->stream=fopen(out->filename, "wb");
    if(!out->stream)
      return -1; /* failure, can't open file to write */ 
  }
  return fwrite(buffer, size, nmemb, out->stream);
}

int download(void){
	CURL *curl;
  CURLcode res;
  struct FtpFile ftpfile={
    "download_Tran.txt", /* name to store the file as if successful */ 
    NULL
  };
 
  curl_global_init(CURL_GLOBAL_DEFAULT);
 
  curl = curl_easy_init();
  if(curl) {
    /*
     * You better replace the URL with one that works!
     */ 
    curl_easy_setopt(curl, CURLOPT_URL,
                     "ftp://ece158a:aloha15markov@52.34.178.188/download_file.txt");
    /* Define our callback to get called when there's data to be written */ 
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_fwrite);
    /* Set a pointer to our struct to pass to the callback */ 
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);
 
    /* Switch on full protocol/debug output */ 
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
 
    res = curl_easy_perform(curl);
 
    /* always cleanup */ 
    curl_easy_cleanup(curl);
 
    if(CURLE_OK != res) {
      /* we failed */ 
      fprintf(stderr, "curl told us %d\n", res);
    }
  }
 
  if(ftpfile.stream)
    fclose(ftpfile.stream); /* close the local file */ 
 
  curl_global_cleanup();
 
  return 0;
}
/*********************************************************************************/

/***************************** Display Listings **********************************/
int displayFolders(){
	CURL *curl;
  CURLcode res = CURLE_OK;
 
  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */ 
    curl_easy_setopt(curl, CURLOPT_USERNAME, "ece158a");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "aloha15markov");
 
    /* This will list the folders within the user's mailbox. If you want to
     * list the folders within a specific folder, for example the inbox, then
     * specify the folder as a path in the URL such as /INBOX */ 
    curl_easy_setopt(curl, CURLOPT_URL, "ftp://52.34.178.188/");
 
    /* Perform the list */ 
    res = curl_easy_perform(curl);
 
    /* Check for errors */ 
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
 
    /* Always cleanup */ 
    curl_easy_cleanup(curl);
  }
 
  return (int)res;
}
/*********************************************************************************/

/* Main */
int main() {

	int n;
	while(n != 4){
		printf("\nWelcome to the ECE158A FTP Client! What would you like to do?\n\n");
		printf("1. Get a remote directory\n2. Send a file\n3. Receive a file\n4. Exit");
		printf("\n\nPlease enter the number for the action you want: ");
		scanf("%d", &n);

		if(n == 4){
			printf("Program will now exit.");
			break;
		}
		else if(n == 1){
			displayFolders();
		}
		else if(n == 2){
			upload();
		}
		else if(n == 3){
			download();
		}
		else {
			printf("Invalid entry, please enter a number 1-4\n");
		}
	}

	return 0;
}
